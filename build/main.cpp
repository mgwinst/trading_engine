#include <print>
#include <algorithm>

#include "network/xdp/xdp_socket.hpp"

int main()
{
    network::xsk_info config {
        .chunk_size_ = 2048,
        .chunk_count_ = 1024,
        .umem_len_ = 2048 * 1024,
        .rx_ring_len_ = 128,
        .fill_ring_len_ = 128,
        .queue_id_ = 0
    };

    std::string_view interface{ "enp3s0f0np0" };

    network::xdp_socket(interface, config);

}

