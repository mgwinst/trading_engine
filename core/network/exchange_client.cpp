#include <boost/lockfree/spsc_queue.hpp>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/if_packet.h>
#include <linux/udp.h>
#include <net/if.h>
#include <thread>

#include "sys/memory.hpp"
#include "common/macros.hpp"
#include "common/allocators.hpp"
#include "socket.hpp"

// VLAN tag 131 (VLAN tagging adds a 4-byte header)
// What is VLAN offloading (strip vlan hdr somehow?)

namespace netutils = network::utilities;

int main() 
{
    int socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    macros::ASSERT(socket_fd != -1, "socket() failed", macros::SOURCE_LOCATION());

    macros::ASSERT(netutils::configure_tpacket_v3(socket_fd) != -1, "configure_tpacket_v3() failed", macros::SOURCE_LOCATION());
    
    tpacket_req3 req{};
    req.tp_block_size = 1 << 22;
    req.tp_block_nr = 8;
    req.tp_frame_size = 2048; // ignored in v3
    req.tp_frame_nr = (req.tp_block_size * req.tp_block_nr) / req.tp_frame_size; // ignored in v3
    req.tp_retire_blk_tov = 60;    // partially filled block is returned to user space (retirement timeout (60ms))
    req.tp_sizeof_priv = 0;        // no private data
    req.tp_feature_req_word = 0;   // no special features

    macros::ASSERT(req.tp_block_size % getpagesize() == 0, 
        "Block size must be multipleof page size", macros::SOURCE_LOCATION());

    macros::ASSERT(netutils::configure_tpacket_v3_rx_buf(socket_fd, req) != -1, "configure_tpacket_v3_rx_ring_buffer() failed", macros::SOURCE_LOCATION());

    sockaddr_ll addr{};
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = if_nametoindex("eno1");
    addr.sll_protocol = htons(ETH_P_ALL);
    addr.sll_pkttype = PACKET_HOST | PACKET_MULTICAST;
    macros::ASSERT(addr.sll_ifindex != 0, "interface index doesn't exist", macros::SOURCE_LOCATION(), errno);

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
                udphdr* udp_hdr = (udphdr*)((char*)pkt + sizeof(ethhdr) + sizeof(iphdr));

                uint16_t len = ntohs(udp_hdr->len);

                if (!queue.push((char*)(&len), sizeof(uint16_t))) {
                    // macros::LOG_ERROR("queue full when pushing UDP datagram byte length", macros::SOURCE_LOCATION());
                    continue;
                }

                if (!queue.push((char*)udp_hdr, ((char*)udp_hdr + len))) {
                    // macros::LOG_ERROR("queue full when pushing UDP datagram", macros::SOURCE_LOCATION());
                    char udp_len_prefix[sizeof(uint16_t)];
                    queue.pop(udp_len_prefix, sizeof(uint16_t));
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

