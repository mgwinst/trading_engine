#pragma once

#include <vector>

#include "socket.hpp"

// we want to be able to set kernel buffer sizes here

namespace network 
{
    
    constexpr int32_t userspace_buffer_size{1 << 20};

    struct TCPSocket 
    {
        TCPSocket() noexcept = default;

        TCPSocket(const TCPSocket&) = delete;
        TCPSocket(const TCPSocket&&) = delete;
        TCPSocket &operator=(const TCPSocket&) = delete;
        TCPSocket &operator=(const TCPSocket&&) = delete;

        void connect();

        int32_t socket_fd_{-1};

        std::array<std::byte, userspace_buffer_size> send_buffer_;
        std::array<std::byte, userspace_buffer_size> recv_buffer_;
        int32_t write_index_{0};
        int32_t read_index_{0};

        
    };
}
