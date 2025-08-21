#pragma once

#include <linux/if_xdp.h>
#include <sys/socket.h>
#include <string_view>
#include <vector>
#include <tuple>

#include "common/allocators.hpp"
#include "common/memory.hpp"
#include "common/macros.hpp"
#include <net/if.h>

using namespace macros;

namespace network {

    inline auto register_umem(std::span<std::byte> buffer) -> int32_t {
        xdp_umem_reg umem_reg {
            .addr = (uint64_t)(void*)buffer.data(),
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

    struct XDPConfig {
        std::size_t chunk_size;
        std::size_t chunk_count;
        std::size_t umem_len;
        RingBufferSizes ring_buf_sizes;
    };

    inline auto configure_xdp_rings(int32_t fd, const RingBufferSizes& rbs) -> int32_t {
        if (setsockopt(fd, SOL_XDP, XDP_RX_RING, &(rbs.rx_ring_len), sizeof(std::size_t)) == -1 || 
            setsockopt(fd, SOL_XDP, XDP_TX_RING, &(rbs.tx_ring_len), sizeof(std::size_t)) == -1 ||
            setsockopt(fd, SOL_XDP, XDP_UMEM_FILL_RING, &(rbs.fill_ring_len), sizeof(std::size_t)) == -1 ||
            setsockopt(fd, SOL_XDP, XDP_UMEM_COMPLETION_RING, &(rbs.completion_ring_len), sizeof(std::size_t)) == -1
        ) return -1;

        return 0;
    }

    inline auto get_offsets(int32_t fd) -> xdp_mmap_offsets {
        xdp_mmap_offsets offsets{};
        socklen_t len{sizeof(offsets)};
        getsockopt(fd, SOL_XDP, XDP_MMAP_OFFSETS, &offsets, &len);
        return offsets;
    }
    
    inline auto mmap_rings(int32_t fd, const XDPConfig& xdp_config, const xdp_mmap_offsets& offsets) -> std::tuple<void*, void*, void*, void*> {
        void* rx_ring_mmap = mmap(nullptr, offsets.rx.desc + xdp_config.ring_buf_sizes.rx_ring_len * sizeof(xdp_desc), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, XDP_PGOFF_RX_RING);
        void* tx_ring_mmap = mmap(nullptr, offsets.rx.desc + xdp_config.ring_buf_sizes.tx_ring_len * sizeof(xdp_desc), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, XDP_PGOFF_TX_RING);
        void* fill_ring_mmap = mmap(nullptr, offsets.fr.desc + xdp_config.ring_buf_sizes.fill_ring_len * sizeof(xdp_desc), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, XDP_UMEM_PGOFF_FILL_RING);
        void* completion_ring_mmap = mmap(nullptr, offsets.cr.desc + xdp_config.ring_buf_sizes.completion_ring_len * sizeof(xdp_desc), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, XDP_UMEM_COMPLETION_RING);
        return {rx_ring_mmap, tx_ring_mmap, fill_ring_mmap, completion_ring_mmap};
    }

    inline auto get_xdp_sockaddr(int32_t fd, std::string_view iface) -> sockaddr_xdp {
        sockaddr_xdp sockaddr {
            .sxdp_family = AF_XDP,
            .sxdp_flags = 0,
            .sxdp_ifindex = if_nametoindex(iface.data()),
            .sxdp_queue_id = 0,
            .sxdp_shared_umem_fd = static_cast<uint32_t>(fd)
        };
        return sockaddr;
    }

    struct XDPSocket {

        XDPSocket(std::string_view iface, const XDPConfig& xdp_config) noexcept {
            umem.resize(xdp_config.umem_len);

            pin_buffer(umem);

            fd = socket(AF_XDP, SOL_XDP, 0);
            macros::ASSERT(fd != -1, "socket() failed", SOURCE_LOCATION(), errno);

            register_umem(umem);
            

            

            
            


            
            
            
        }


        ~XDPSocket() {
            unpin_buffer(umem);

            // munmap rings

            if (fd) close(fd);
        }


        int32_t fd{-1};
        std::vector<std::byte, PageAlignedAllocator<std::byte>> umem;

        // buffers

        std::size_t chunk_size;
        std::size_t chunk_count;
        std::size_t umem_len;
        std::size_t ring_buf_len;
        
        
    };

    


}
