#include <linux/if_xdp.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <net/if.h>
#include <cstdint>
#include <string_view>
#include <expected>

#include "xdp_socket.hpp"

namespace network 
{
    xdp_socket::xdp_socket(std::string_view interface, const xsk_info& config) noexcept
    {
        info_ = config;
        
        umem_.resize(config.umem_len_);
        pin_buffer(umem_);

        macros::ASSERT((fd_ = socket(AF_XDP, SOCK_RAW, 0)) != -1, "socket()", SOURCE_LOCATION());

        macros::ASSERT(register_umem() != -1, "register_umem()", SOURCE_LOCATION());
        macros::ASSERT(map_rings() != -1, "map_rings()", SOURCE_LOCATION(), errno);

        auto sockaddr{ get_xsk_addr(interface) };
        macros::ASSERT(bind(fd_, (struct sockaddr*)&sockaddr, sizeof(sockaddr_xdp)) != -1, "bind()", SOURCE_LOCATION(), errno); // will fail until xdp program is loaded onto interface
    }

    xdp_socket::~xdp_socket() noexcept
    {
        unpin_buffer(umem_);
        unmap_rings();

        if (fd_)
            close(fd_);
    }

    auto xdp_socket::register_umem() -> int32_t
    {
        xdp_umem_reg umem_reg {
            .addr = reinterpret_cast<uint64_t>(static_cast<void*>(umem_.data())),
            .len = info_.umem_len_,
            .chunk_size = static_cast<uint32_t>(info_.chunk_size_),
            .headroom = 0,
            .flags = 0
        };

        if (setsockopt(fd_, SOL_XDP, XDP_UMEM_REG, &umem_reg, sizeof(umem_reg)) == -1)
            return -1;

        return 0;
    }

    auto xdp_socket::map_rings() -> int32_t
    {
        setsockopt(fd_, SOL_XDP, XDP_RX_RING, &(info_.rx_ring_len_), sizeof(info_.rx_ring_len_));
        setsockopt(fd_, SOL_XDP, XDP_UMEM_FILL_RING, &(info_.fill_ring_len_), sizeof(info_.fill_ring_len_));

        xdp_mmap_offsets offsets{};
        socklen_t len{ sizeof(offsets) };
        getsockopt(fd_, SOL_XDP, XDP_MMAP_OFFSETS, &offsets, &len);

        info_.offsets_ = offsets;

        rings_.rx_ring_mmap_ = mmap(nullptr, info_.offsets_.rx.desc + info_.rx_ring_len_ * sizeof(xdp_desc), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd_, XDP_PGOFF_RX_RING);
        rings_.fill_ring_mmap_ = mmap(nullptr, info_.offsets_.fr.desc + info_.fill_ring_len_ * sizeof(xdp_desc), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd_, XDP_UMEM_PGOFF_FILL_RING);

        if (rings_.rx_ring_mmap_ == MAP_FAILED ||
            rings_.fill_ring_mmap_ == MAP_FAILED
        ) return -1;

        rings_.rx_ring_consumer_ = reinterpret_cast<uint32_t*>(static_cast<char*>(rings_.rx_ring_mmap_) + info_.offsets_.rx.consumer);
        rings_.rx_ring_producer_ = reinterpret_cast<uint32_t*>(static_cast<char*>(rings_.rx_ring_mmap_) + info_.offsets_.rx.producer);
        rings_.fill_ring_consumer_ = reinterpret_cast<uint32_t*>(static_cast<char*>(rings_.fill_ring_mmap_) + info_.offsets_.fr.consumer);
        rings_.fill_ring_producer_ = reinterpret_cast<uint32_t*>(static_cast<char*>(rings_.fill_ring_mmap_) + info_.offsets_.fr.producer);
        
        rings_.rx_ring_ = reinterpret_cast<xdp_desc*>(static_cast<char*>(rings_.rx_ring_mmap_) + info_.offsets_.rx.desc);
        rings_.fill_ring_ = reinterpret_cast<xdp_desc*>(static_cast<char*>(rings_.fill_ring_mmap_) + info_.offsets_.fr.desc);

        return 0;
    } 

    inline auto xdp_socket::unmap_rings() -> int32_t
    {
        if (munmap(rings_.rx_ring_mmap_, info_.offsets_.rx.desc + info_.rx_ring_len_ * sizeof(xdp_desc)) == -1 ||
            munmap(rings_.fill_ring_mmap_, info_.offsets_.fr.desc + info_.fill_ring_len_ * sizeof(xdp_desc) == -1)
        ) return -1;

        return 0;
    }

    auto xdp_socket::get_xsk_addr(std::string_view interface) -> sockaddr_xdp 
    {
        return sockaddr_xdp {
            .sxdp_family = AF_XDP,
            .sxdp_flags = 0,
            .sxdp_ifindex = if_nametoindex(interface.data()),
            .sxdp_queue_id = static_cast<uint32_t>(info_.queue_id_),
            .sxdp_shared_umem_fd = static_cast<uint32_t>(fd_)
        };
    }

} // end of namespace


    