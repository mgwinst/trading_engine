#pragma once

#include <linux/ip.h>
#include <linux/udp.h>
#include <sys/epoll.h>
#include <memory>
#include <vector>
#include <atomic>
#include <thread>

#include "raw_socket.hpp"

namespace network
{
    class FeedHandler 
    {
    public:
        FeedHandler(std::shared_ptr<RawSocket>& socket);
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
        int32_t epoll_fd_{ -1 };
        epoll_event events_[1];
        std::thread* rx_thread_{ nullptr };

        void add_to_epoll_list(std::shared_ptr<RawSocket>& socket);
    };

} // namespace network
