#include <linux/if_xdp.h>
#include <sys/socket.h>
#include <cstddef>
#include <cstdlib>
#include <vector>

#include "common/macros.hpp"

using namespace macros;

namespace {
    constexpr std::size_t chunk_size{4096};
    constexpr std::size_t chunk_count{4096};
    constexpr std::size_t umem_len{chunk_count * chunk_size};
}

int main() {
    std::vector<std::byte> umem;
    umem.resize(umem_len);

    int fd = socket(AF_XDP, SOCK_RAW, 0);

    xdp_umem_reg umem_reg {
        .addr = (__u64)(void*)umem.data(),
        .len = umem_len,
        .chunk_size = chunk_size,
        .headroom = 0,
        .flags = 0
    };

    macros::ASSERT(setsockopt(fd, SOL_XDP, XDP_UMEM_REG, &umem_reg, sizeof(umem_reg)) != -1,
        "setsockopt() failed", SOURCE_LOCATION());
    
    
}