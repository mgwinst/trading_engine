#pragma once

#include <linux/ip.h>
#include <linux/udp.h>
#include <sys/epoll.h>
#include <memory>
#include <atomic>
#include <thread>

#include "socket.hpp"
#include "../common/CoreSet.hpp"

namespace network
{
    class FeedHandler
    {
    public:
        template <typename... Args>
        FeedHandler(Args... args)
        {
            rx_socket_ = std::make_shared<RawSocket>(args...);

            epoll_fd_ = epoll_create(1);

            if (epoll_fd_ == -1)
                error_exit("epoll_create()");

            add_to_epoll_list(rx_socket_);
        }

        ~FeedHandler();

        FeedHandler(const FeedHandler&) = delete; 
        FeedHandler& operator=(const FeedHandler&) = delete; 
        FeedHandler(FeedHandler&&) = delete; 
        FeedHandler& operator=(FeedHandler&&) = delete; 

        void start_rx();
        void stop_rx();

    private:
        std::atomic<bool> running_{ false };
        std::shared_ptr<RawSocket> rx_socket_;
        std::jthread rx_thread_;
        int32_t epoll_fd_{ -1 };
        epoll_event events_[1];
        CoreID claimed_core_;

        void add_to_epoll_list(std::shared_ptr<RawSocket>& socket);
    };
    
} // namespace network
