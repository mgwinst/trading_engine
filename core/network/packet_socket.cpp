#include "sys/memory.hpp"
#include "common/macros.hpp"
#include "common/allocators.hpp"
#include "socket.hpp"

#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <boost/lockfree/spsc_queue.hpp>
#include <netinet/udp.h>
#include <thread>

#define ETHERNET_HDR_LEN 14
#define IP_HDR_LEN 20
#define UDP_HDR_LEN 8
#define UDP_HDR_OFFSET (ETHERNET_HDR_LEN + IP_HDR_LEN)
#define PAYLOAD_OFFSET (ETHERNET_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN)

int main() 
{
    int socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    if (socket_fd == -1) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    int version = TPACKET_V3;
    if (setsockopt(socket_fd, SOL_PACKET, PACKET_VERSION, &version, sizeof(version)) == -1) {
        perror("setsockopt() failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    tpacket_req3 req{};
    req.tp_block_size = 4096;
    req.tp_block_nr = 8;
    req.tp_frame_size = 2048; // ignored in v3
    req.tp_frame_nr = (req.tp_block_size * req.tp_block_nr) / req.tp_frame_size; // ignored in v3
    req.tp_retire_blk_tov = 60;    // partially filled block is returned to user space (retirement timeout (60ms))
    req.tp_sizeof_priv = 0;        // no private data
    req.tp_feature_req_word = 0;   // no special features

    if (req.tp_block_size % getpagesize() != 0) {
        std::print(std::cerr, "Block size must be multipleof page size");
    }

    if (setsockopt(socket_fd, SOL_PACKET, PACKET_RX_RING, &req, sizeof(req)) == -1) {
        perror("setsockopt() failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    sockaddr_ll addr{};
    addr.sll_family = AF_PACKET;
    addr.sll_protocol = htons(ETH_P_IP);
    addr.sll_ifindex = if_nametoindex("eno1"); // make sure != 0
    if (addr.sll_ifindex == 0) {
        perror("if_nametoindex() failed.");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    if (bind(socket_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
        perror("bind() failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    void* ring_buffer = mmap(nullptr, req.tp_block_size * req.tp_block_nr, PROT_READ | PROT_WRITE, MAP_SHARED, socket_fd, 0);   
    if (ring_buffer == MAP_FAILED) {
        perror("mmap() failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    // std::print("socket ring buffer successfully mapped.\n");

    boost::lockfree::spsc_queue<char, boost::lockfree::capacity<(1<<20)>> queue;

    tpacket_block_desc* block = (tpacket_block_desc*)(ring_buffer);
    int block_idx = 0;
    
    while (1) {
        if (block[block_idx].hdr.bh1.block_status & TP_STATUS_USER) {
            tpacket3_hdr* pkt = (tpacket3_hdr*)((char*)block + block[block_idx].hdr.bh1.offset_to_first_pkt);
            for (int i = 0; i < block[block_idx].hdr.bh1.num_pkts; i++) {
                pkt = (tpacket3_hdr*)((char*)pkt + pkt->tp_next_offset);
                udphdr* udp_hdr = (udphdr*)((char*)pkt + UDP_HDR_OFFSET);

                uint16_t len = ntohs(udp_hdr->len);

                if (!queue.push((char*)(&len), sizeof(len))) {
                    macros::LOG_ERROR("queue full", macros::SOURCE_LOCATION());
                    continue;
                }

                if (!queue.push((char*)udp_hdr, ((char*)udp_hdr + len))) {
                    macros::LOG_ERROR("queue full when pushing UDP datagram", macros::SOURCE_LOCATION());
                    char udp_len_prefix[sizeof(len)];
                    queue.pop(udp_len_prefix, sizeof(len));
                    continue;
                }
            }
            block[block_idx].hdr.bh1.block_status = TP_STATUS_KERNEL;
            block_idx = (block_idx + 1) % req.tp_block_nr;
        }
    }

    
    munmap(ring_buffer, req.tp_block_size * req.tp_block_nr);
    close(socket_fd);

    return 0;
}

