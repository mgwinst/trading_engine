#include "sys/memory.hpp"
#include "common/macros.hpp"
#include "common/allocators.hpp"
#include "socket.hpp"

#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>

// ethernet header, ipv4 header, payload

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
    req.tp_frame_size = 2048;
    req.tp_frame_nr = (req.tp_block_size * req.tp_block_nr) / req.tp_frame_size;
    req.tp_retire_blk_tov = 60;    // block retirement timeout (60ms)
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
 
    void* map = mmap(nullptr, req.tp_block_size * req.tp_block_nr, PROT_READ | PROT_WRITE, MAP_SHARED, socket_fd, 0);   
    if (map == MAP_FAILED) {
        perror("mmap() failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    sockaddr_ll addr{};
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = if_nametoindex("eno1"); // make sure != 0
    addr.sll_protocol = htons(ETH_P_IP);

    if (bind(socket_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
        perror("bind() failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    std::print("socket ring buffer successfully mapped.\n");

    munmap(map, req.tp_block_size * req.tp_block_nr);
    close(socket_fd);


    // do we specify ring buffer options on both rx and tx together? Sep buffers? 




    return 0;
}

