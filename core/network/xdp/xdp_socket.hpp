#pragma once

#include <linux/if_xdp.h>
#include <sys/socket.h>
#include <vector>
#include <string_view>

#include "common/allocators.hpp"
#include "common/memory.hpp"

namespace network {

    template <typename T>
    concept BufferPointer = std::is_pointer_v<T> && std::convertible_to<T, void *>;

    inline auto register_umem(BufferPointer auto* umem) -> int32_t {
        xdp_umem_reg umem_reg {
            .addr = (uint64_t)(void*)umem,
            .len = umem_len,
            .chunk_size = chunk_size,
            .headroom = 0,
            .flags = 0
        };

        if (setsockopt(fd, SOL_XDP, XDP_UMEM_REG, &umem_reg, sizeof(umem_reg)) == -1)
            return -1;

        return 0;
    }

    struct RingBufferSizes {
        std::size_t rx_ring_len;
        std::size_t tx_ring_len;
        std::size_t fill_ring_len;
        std::size_t completion_ring_len;
    };

    inline auto configure_xdp_rings(int32_t fd, const RingBufferSizes& rbs) -> int32_t {
        if (setsockopt(fd, SOL_XDP, XDP_RX_RING, &(rbs.rx_ring_len), sizeof(std::size_t)) == -1 || 
            setsockopt(fd, SOL_XDP, XDP_TX_RING, &(rbs.tx_ring_len), sizeof(std::size_t)) == -1 ||
            setsockopt(fd, SOL_XDP, XDP_UMEM_FILL_RING, &(rbs.fill_ring_len), sizeof(std::size_t)) == -1 ||
            setsockopt(fd, SOL_XDP, XDP_UMEM_COMPLETION_RING, &(rbs.completion_ring_len), sizeof(std::size_t)) == -1
        ) return -1;

        return 0;
    }
    
    struct XDPConfig {
        std::size_t chunk_size;
        std::size_t chunk_count;
        std::size_t umem_len;
        RingBufferSizes ring_buf_sizes;
    };

    struct XDPSocket {
        
        XDPSocket(std::string_view iface, const XDPConfig& xdp_config) noexcept {
            fd = socket(AF_XDP, )

        }


        int32_t fd{-1};
        std::vector<std::byte, PageAlignedAllocator<std::byte>> umem;
        std::size_t chunk_size;
        std::size_t chunk_count;
        std::size_t umem_len;
        std::size_t ring_buf_len;
        
        
    };

    


}
