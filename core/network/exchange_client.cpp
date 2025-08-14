#include <boost/lockfree/spsc_queue.hpp>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <netinet/udp.h>
#include <net/if.h>
#include <thread>

#include "sys/memory.hpp"
#include "common/macros.hpp"
#include "common/allocators.hpp"
#include "socket.hpp"

#define ETHERNET_HDR_LEN 14
#define IP_HDR_LEN 20
#define UDP_HDR_LEN 8
#define UDP_HDR_OFFSET (ETHERNET_HDR_LEN + IP_HDR_LEN)
#define PAYLOAD_OFFSET (ETHERNET_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN)

namespace netutils = network::utilities;

int main() 
{
    int socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    macros::ASSERT(socket_fd != -1, "socket() failed", macros::SOURCE_LOCATION());

    macros::ASSERT(netutils::configure_tpacket_v3(socket_fd) != -1, "configure_tpacket_v3() failed", macros::SOURCE_LOCATION());
    
    auto req = netutils::create_tpacket_req(1<<22, 8, 60);

    macros::ASSERT(netutils::configure_tpacket_v3_rx_buf(socket_fd, req) != -1, "configure_tpacket_v3_rx_ring_buffer() failed", macros::SOURCE_LOCATION());
    
    auto addr = netutils::get_sockaddr_ll("eno1", AF_PACKET, ETH_P_IP);

    macros::ASSERT(bind(socket_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != -1, "bind() failed", macros::SOURCE_LOCATION());

    void* ring_buffer = mmap(nullptr, req.tp_block_size * req.tp_block_nr, PROT_READ | PROT_WRITE, MAP_SHARED, socket_fd, 0);   
    macros::ASSERT(ring_buffer != MAP_FAILED, "mmap() failed", macros::SOURCE_LOCATION());

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
                    macros::LOG_ERROR("queue full when pushing UDP datagram byte length", macros::SOURCE_LOCATION());
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

