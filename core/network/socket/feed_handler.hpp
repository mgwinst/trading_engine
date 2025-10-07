#pragma once

#include <linux/ip.h>
#include <linux/udp.h>
#include <sys/epoll.h>
#include <memory>
#include <vector>
#include <utility>
#include <atomic>
#include <thread>

#include "network/socket/raw_socket.hpp"
#include "common/CoreSet.hpp"

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
        std::jthread rx_thread_;
        CoreID claimed_core_;
        int32_t epoll_fd_{ -1 };
        epoll_event events_[1];

        void add_to_epoll_list(std::shared_ptr<RawSocket>& socket);
    };
    
} // namespace network
