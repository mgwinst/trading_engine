#pragma once

#include <sys/socket.h>
#include <string_view>
#include <vector>

#include "xdp_utils.hpp"
#include "common/allocators.hpp" // memory and allocators should be in one header?
#include "common/memory.hpp"
#include "common/macros.hpp"

using namespace macros;

namespace network 
{
    class xdp_socket {
    public:
        xdp_socket(std::string_view interface, const xsk_info& config) noexcept;

        xdp_socket(const xdp_socket&) = delete;
        xdp_socket& operator=(const xdp_socket&) = delete;
        xdp_socket(xdp_socket&&) = delete;
        xdp_socket& operator=(xdp_socket&&) = delete;
        ~xdp_socket() noexcept;

        auto recvfrom() noexcept -> void;

    private:
        template <typename T> using umem_buffer = std::vector<T, PageAlignedAllocator<T>>;

        int32_t fd_{-1};
        umem_buffer<std::byte> umem_;
        xsk_info info_;
        xsk_rings rings_;

        auto get_xsk_addr(std::string_view interface) -> sockaddr_xdp;
        auto register_umem() -> int32_t;
        auto map_rings() -> int32_t;
        auto unmap_rings() -> int32_t;

    };

} // end of namespace
