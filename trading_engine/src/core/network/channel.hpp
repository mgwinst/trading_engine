#pragma once

#include <linux/ip.h>
#include <linux/udp.h>
#include <sys/epoll.h>
#include <memory>
#include <atomic>
#include <thread>
#include <unistd.h>

#include "socket.hpp"
#include "../common/cores.hpp"

namespace network
{
    class Channel
    {
    public:
        template <typename... Args>
        Channel(Args... args)
        {
            socket_ = std::make_shared<RawSocket>(args...);

            epoll_fd_ = epoll_create(1);

            if (epoll_fd_ == -1)
                error_exit("epoll_create()");

            add_to_epoll_list(socket_);
        }

        ~Channel()
        {
            if (running_.load())
                stop_rx();

            if (epoll_fd_)
                close(epoll_fd_);
        }

        Channel(const Channel&) = delete; 
        Channel& operator=(const Channel&) = delete; 
        Channel(Channel&&) = delete; 
        Channel& operator=(Channel&&) = delete; 

        void start_rx();
        void stop_rx();

    private:
        std::atomic<bool> running_{ false };
        std::shared_ptr<RawSocket> socket_;
        std::jthread rx_thread_;
        int32_t epoll_fd_{ -1 };
        epoll_event events_[1];
        CoreID claimed_core_;

        void add_to_epoll_list(std::shared_ptr<RawSocket>& socket) noexcept;
        void on_event() noexcept;
    };
    
} // namespace network
