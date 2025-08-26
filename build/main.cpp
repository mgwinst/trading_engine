#include "../core/network/xdp_socket.hpp"
#include <cassert>

using namespace network;

int main() 
{
    XDPConfig xdp_config {
        .chunk_size = 4096,
        .chunk_count = 4096,
        .umem_len = (4096 * 4096),
        .ring_buf_sizes = {512, 512, 512, 512}
    };

    XDPSocket xdp_socket{"eno1", xdp_config};

   
}

