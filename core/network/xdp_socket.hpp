#pragma once

#include "xdp_utils.hpp"
#include "../common/allocators.hpp"
#include "../common/memory.hpp"
#include "../common/macros.hpp"

using namespace macros;

namespace network {

    struct xdp_socket {

        xdp_socket(std::string_view iface, const xsk_config& xdp_config) noexcept 
        {
            umem_.resize(xdp_config.umem_len);
            pin_buffer(umem_);
            macros::ASSERT((fd_ = socket(AF_XDP, SOCK_RAW, 0)) != -1, "socket()", SOURCE_LOCATION());
            macros::ASSERT(register_umem(fd_, umem_, xdp_config) != -1, "register_umem()", SOURCE_LOCATION());
            macros::ASSERT(set_xsk_ring_sizes(fd_, xdp_config.ring_buf_sizes) != -1, "set_xsk_ring_sizes()", SOURCE_LOCATION());
            auto offsets{ get_xdp_mmap_offsets(fd_) };
            macros::ASSERT(init_rings(fd_, xdp_config, offsets, rings_) != -1, "init_rings()", SOURCE_LOCATION(), errno);
            auto sockaddr{ get_xdp_sockaddr(fd_, "eno1") };
            macros::ASSERT(bind(fd_, (struct sockaddr*)&sockaddr, sizeof(struct sockaddr_xdp)) != -1, "bind()", SOURCE_LOCATION(), errno);
        }

        ~xdp_socket() noexcept 
        {
            unpin_buffer(umem_);
            // munmap rings
            if (fd_) close(fd_);
        }

        xdp_socket(const xdp_socket&) = delete;
        xdp_socket& operator=(const xdp_socket&) = delete;
        xdp_socket(xdp_socket&&) = delete;
        xdp_socket& operator=(xdp_socket&&) = delete;

        auto recvfrom() noexcept -> void;

        int32_t fd_{ -1 };
        std::vector<std::byte, PageAlignedAllocator<std::byte>> umem_;
        Rings rings_;
        std::size_t chunk_size_;
        std::size_t chunk_count_;
        std::size_t umem_len_;
        std::size_t ring_buf_len_;
        
    };

    


}
