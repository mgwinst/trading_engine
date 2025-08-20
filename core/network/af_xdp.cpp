#include <linux/if_xdp.h>
#include <sys/socket.h>
#include <cstddef>
#include <cstdlib>
#include <vector>
#include <print>

#include "common/allocators.hpp"
#include "common/macros.hpp"
#include "sys/memory.hpp"

using namespace macros;

namespace {
    constexpr std::size_t chunk_size{4096};
    constexpr std::size_t chunk_count{4096};
    constexpr std::size_t umem_len{chunk_count * chunk_size};
    constexpr std::size_t ring_buf_len{512};
}

int main() {
    std::vector<std::byte, PageAlignedAllocator<std::byte>> umem;
    umem.resize(umem_len);

    pin_buffer(umem);

    int32_t fd = socket(AF_XDP, SOCK_RAW, 0);
    macros::ASSERT(fd != -1, "socket() failed", SOURCE_LOCATION(), errno);

    xdp_umem_reg umem_reg {
        .addr = (__u64)(void*)umem.data(),
        .len = umem_len,
        .chunk_size = chunk_size,
        .headroom = 0,
        .flags = 0
    };

    macros::ASSERT(setsockopt(fd, SOL_XDP, XDP_UMEM_REG, &umem_reg, sizeof(umem_reg)) != -1,
        "setsockopt() failed", SOURCE_LOCATION(), errno);

    macros::ASSERT(setsockopt(fd, SOL_XDP, XDP_RX_RING, &ring_buf_len, sizeof(ring_buf_len)) != -1, "setsockopt() failed", SOURCE_LOCATION(), errno);
    macros::ASSERT(setsockopt(fd, SOL_XDP, XDP_TX_RING, &ring_buf_len, sizeof(ring_buf_len)) != -1, "setsockopt() failed", SOURCE_LOCATION(), errno);
    macros::ASSERT(setsockopt(fd, SOL_XDP, XDP_UMEM_FILL_RING, &ring_buf_len, sizeof(ring_buf_len)) != -1, "setsockopt() failed", SOURCE_LOCATION(), errno);
    macros::ASSERT(setsockopt(fd, SOL_XDP, XDP_UMEM_COMPLETION_RING, &ring_buf_len, sizeof(ring_buf_len)) != -1, "setsockopt() failed", SOURCE_LOCATION(), errno);

    xdp_mmap_offsets offsets{};
    socklen_t offsets_len{sizeof(offsets)};
    
    macros::ASSERT(getsockopt(fd, SOL_XDP, XDP_MMAP_OFFSETS, &offsets, &offsets_len) != -1, "getsockopt() failed", SOURCE_LOCATION(), errno);
    
    void* rx_ring_mmap = mmap(nullptr, offsets.rx.desc + ring_buf_len * sizeof(xdp_desc), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, XDP_PGOFF_RX_RING);
    void* tx_ring_mmap = mmap(nullptr, offsets.rx.desc + ring_buf_len * sizeof(xdp_desc), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, XDP_PGOFF_TX_RING);
    void* fill_ring_mmap = mmap(nullptr, offsets.fr.desc + ring_buf_len * sizeof(xdp_desc), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, XDP_UMEM_PGOFF_FILL_RING);
    void* completion_ring_mmap = mmap(nullptr, offsets.cr.desc + ring_buf_len * sizeof(xdp_desc), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, XDP_UMEM_COMPLETION_RING);

    macros::ASSERT(rx_ring_mmap != MAP_FAILED ||
                   tx_ring_mmap != MAP_FAILED ||
                   fill_ring_mmap != MAP_FAILED ||
                   completion_ring_mmap != MAP_FAILED,
                   "mmap() ring failed", SOURCE_LOCATION(), errno);

    uint32_t* rx_ring_consumer = (uint32_t*)((char*)rx_ring_mmap + offsets.rx.consumer);
    uint32_t* rx_ring_producer = (uint32_t*)((char*)rx_ring_mmap + offsets.rx.producer);
    uint32_t* tx_ring_consumer = (uint32_t*)((char*)tx_ring_mmap + offsets.tx.consumer);
    uint32_t* tx_ring_producer = (uint32_t*)((char*)tx_ring_mmap + offsets.tx.producer);
    uint32_t* fill_ring_consumer = (uint32_t*)((char*)fill_ring_mmap + offsets.fr.consumer);
    uint32_t* fill_ring_producer = (uint32_t*)((char*)fill_ring_mmap + offsets.fr.producer);
    uint32_t* completion_ring_consumer = (uint32_t*)((char*)completion_ring_mmap + offsets.cr.consumer);
    uint32_t* completion_ring_producer = (uint32_t*)((char*)completion_ring_mmap + offsets.cr.producer);

    xdp_desc* rx_ring = (xdp_desc*)((char*)rx_ring_mmap + offsets.rx.desc);
    xdp_desc* tx_ring = (xdp_desc*)((char*)tx_ring_mmap + offsets.tx.desc);
    xdp_desc* fill_ring = (xdp_desc*)((char*)fill_ring_mmap + offsets.fr.desc);
    xdp_desc* completion_ring = (xdp_desc*)((char*)completion_ring_mmap + offsets.cr.desc);
    
}