#pragma once

#include <linux/if_packet.h>
#include <string_view>
#include <cstdint>

#include "tpacket.hpp"
#include "socket_utils.hpp"
#include "../common/macros.hpp"

namespace network
{
    class RawSocket
    {
    public:
        RawSocket(std::string_view interface)
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

            processor_ = std::make_unique<TPacketProcessor>(socket_fd);

            fd_ = socket_fd;
        }

        ~RawSocket()
        {
            if (fd_)
                close(fd_);
        }

        RawSocket(const RawSocket&) = delete;
        RawSocket& operator=(const RawSocket&) = delete;
        RawSocket(RawSocket&&) = delete;
        RawSocket& operator=(RawSocket&&) = delete;

        void read() noexcept
        {
            processor_->drain_buffer();
        }

        auto get_fd() const noexcept 
        { 
            return fd_; 
        }

    private:
        int32_t fd_{ -1 };
        std::unique_ptr<TPacketProcessor> processor_;
    };

} // namespace network