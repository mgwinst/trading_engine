#pragma once

#include <linux/if_xdp.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <net/if.h>
#include <cstdint>
#include <string_view>
#include <span>

namespace network {

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

    struct Rings {
        void* rx_ring_mmap{ nullptr };
        void* tx_ring_mmap{ nullptr };
        void* fill_ring_mmap{ nullptr };
        void* completion_ring_mmap{ nullptr };

        uint32_t* rx_ring_consumer{ nullptr };
        uint32_t* rx_ring_producer{ nullptr };
        uint32_t* tx_ring_consumer{ nullptr };
        uint32_t* tx_ring_producer{ nullptr };
        uint32_t* fill_ring_consumer{ nullptr };
        uint32_t* fill_ring_producer{ nullptr };
        uint32_t* completion_ring_consumer{ nullptr };
        uint32_t* completion_ring_producer{ nullptr };

        xdp_desc* rx_ring{ nullptr };
        xdp_desc* tx_ring{ nullptr };
        xdp_desc* fill_ring{ nullptr };
        xdp_desc* completion_ring{ nullptr };
    };

    inline auto register_umem(int32_t fd, std::span<std::byte> buffer, const XDPConfig& xdp_config) -> int32_t {
        xdp_umem_reg umem_reg {
            .addr = reinterpret_cast<uint64_t>(static_cast<void*>(buffer.data())),
            .len = xdp_config.umem_len,
            .chunk_size = static_cast<uint32_t>(xdp_config.chunk_size),
            .headroom = 0,
            .flags = 0
        };

        if (setsockopt(fd, SOL_XDP, XDP_UMEM_REG, &umem_reg, sizeof(umem_reg)) == -1)
            return -1;

        return 0;
    }

    inline auto set_xsk_ring_sizes(int32_t fd, const RingBufferSizes& rbs) -> int32_t {
        if (setsockopt(fd, SOL_XDP, XDP_RX_RING, &(rbs.rx_ring_len), sizeof(std::size_t)) == -1 || 
            setsockopt(fd, SOL_XDP, XDP_TX_RING, &(rbs.tx_ring_len), sizeof(std::size_t)) == -1 ||
            setsockopt(fd, SOL_XDP, XDP_UMEM_FILL_RING, &(rbs.fill_ring_len), sizeof(std::size_t)) == -1 ||
            setsockopt(fd, SOL_XDP, XDP_UMEM_COMPLETION_RING, &(rbs.completion_ring_len), sizeof(std::size_t)) == -1
        ) return -1;

        return 0;
    }

    inline auto get_xdp_mmap_offsets(int32_t fd) -> xdp_mmap_offsets {
        xdp_mmap_offsets offsets{};
        socklen_t len{sizeof(offsets)};
        getsockopt(fd, SOL_XDP, XDP_MMAP_OFFSETS, &offsets, &len);
        return offsets;
    }

    inline auto init_rings(int32_t fd, const XDPConfig& xdp_config, const xdp_mmap_offsets& offsets, Rings& rings) -> int32_t {
        rings.rx_ring_mmap = mmap(nullptr, offsets.rx.desc + xdp_config.ring_buf_sizes.rx_ring_len * sizeof(xdp_desc), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, XDP_PGOFF_RX_RING);
        rings.tx_ring_mmap = mmap(nullptr, offsets.rx.desc + xdp_config.ring_buf_sizes.tx_ring_len * sizeof(xdp_desc), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, XDP_PGOFF_TX_RING);
        rings.fill_ring_mmap = mmap(nullptr, offsets.fr.desc + xdp_config.ring_buf_sizes.fill_ring_len * sizeof(xdp_desc), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, XDP_UMEM_PGOFF_FILL_RING);
        rings.completion_ring_mmap = mmap(nullptr, offsets.cr.desc + xdp_config.ring_buf_sizes.completion_ring_len * sizeof(xdp_desc), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, XDP_UMEM_COMPLETION_RING);

        if (rings.rx_ring_mmap == MAP_FAILED ||
            rings.tx_ring_mmap == MAP_FAILED ||
            rings.fill_ring_mmap == MAP_FAILED ||
            rings.completion_ring_mmap == MAP_FAILED
        ) return -1;

        rings.rx_ring_consumer = reinterpret_cast<uint32_t*>(static_cast<char*>(rings.rx_ring_mmap) + offsets.rx.consumer);
        rings.rx_ring_producer = reinterpret_cast<uint32_t*>(static_cast<char*>(rings.rx_ring_mmap) + offsets.rx.producer);
        rings.tx_ring_consumer = reinterpret_cast<uint32_t*>(static_cast<char*>(rings.tx_ring_mmap) + offsets.tx.consumer);
        rings.tx_ring_producer = reinterpret_cast<uint32_t*>(static_cast<char*>(rings.tx_ring_mmap) + offsets.tx.producer);
        rings.fill_ring_consumer = reinterpret_cast<uint32_t*>(static_cast<char*>(rings.fill_ring_mmap) + offsets.fr.consumer);
        rings.fill_ring_producer = reinterpret_cast<uint32_t*>(static_cast<char*>(rings.fill_ring_mmap) + offsets.fr.producer);
        rings.completion_ring_consumer = reinterpret_cast<uint32_t*>(static_cast<char*>(rings.completion_ring_mmap) + offsets.cr.consumer);
        rings.completion_ring_producer = reinterpret_cast<uint32_t*>(static_cast<char*>(rings.completion_ring_mmap) + offsets.cr.producer);

        rings.rx_ring = reinterpret_cast<xdp_desc*>(static_cast<char*>(rings.rx_ring_mmap) + offsets.rx.desc);
        rings.tx_ring = reinterpret_cast<xdp_desc*>(static_cast<char*>(rings.tx_ring_mmap) + offsets.tx.desc);
        rings.fill_ring = reinterpret_cast<xdp_desc*>(static_cast<char*>(rings.fill_ring_mmap) + offsets.fr.desc);
        rings.completion_ring = reinterpret_cast<xdp_desc*>(static_cast<char*>(rings.completion_ring_mmap) + offsets.cr.desc);

        return 0;
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

    inline auto create_and_set_xdp_socket(const XDPConfig& xdp_config) -> int32_t;

}


    