#include <print>
#include <algorithm>

#include "network/xdp_socket.hpp"

int main()
{
    network::XDPConfig xdp_config {
        .chunk_size = 2048,
        .chunk_count = 1024,
        .umem_len = 2048 * 1024,
        .ring_buf_sizes = {
            .rx_ring_len = 128,
            .tx_ring_len = 128,
            .fill_ring_len = 128,
            .completion_ring_len = 128
        }
    };

    std::string_view interface{ "enp3s0f0np0" };

    network::XDPSocket(interface, xdp_config);

}

