#pragma once

#include "socket_utils.hpp"
#include <concepts>
#include <linux/if_packet.h>
#include <string_view>
#include <cstdint>

#include "../common/macros.hpp"

namespace network
{
    template <typename T>
    concept PacketProcessor = std::invocable<T, std::span<std::byte>>;

    template <PacketProcessor Processor>
    class RawSocket
    {
    public:
        RawSocket(std::string_view interface);
        ~RawSocket();

        RawSocket(const RawSocket&) = delete;
        RawSocket& operator=(const RawSocket&) = delete;
        RawSocket(RawSocket&&) = delete;
        RawSocket& operator=(RawSocket&&) = delete;

        void read() noexcept
        {
            processor_();
        }

        auto get_fd() const noexcept 
        { 
            return fd_; 
        }

    private:
        int32_t fd_{ -1 };
        Processor processor_;
    };

    template <PacketProcessor Processor>
    RawSocket<Processor>::RawSocket(std::string_view interface)
    {
        if (!interface_exists(interface.data()))
            error_exit("interface doesn't exist");

        int32_t socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

        if (socket_fd == -1)
            error_exit("socket()");

        if (set_non_blocking(socket_fd))
            error_exit("set_non_blocking()");

        if (bind_to_interface(socket_fd, interface.data()))
            error_exit("bind_to_interface()");

        fd_ = socket_fd;
    }

    template <PacketProcessor Processor>
    RawSocket<Processor>::~RawSocket()
    {
        if (fd_)
            close(fd_);
    }

} // namespace network