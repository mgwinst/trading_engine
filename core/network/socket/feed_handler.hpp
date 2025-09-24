#pragma once

#include <linux/ip.h>
#include <linux/udp.h>
#include <sys/epoll.h>
#include <memory>
#include <vector>
#include <atomic>

#include "raw_socket.hpp"
#include "itch/moldudp64.hpp"
#include "common/queues/CircularBuffer.hpp"

namespace network
{
    class FeedHandler 
    {
    public:
        FeedHandler();
        ~FeedHandler();

        FeedHandler(const FeedHandler&) = delete; 
        FeedHandler& operator=(const FeedHandler&) = delete; 
        FeedHandler(FeedHandler&&) = delete; 
        FeedHandler& operator=(FeedHandler&&) = delete; 

        void add_to_epoll_list(std::shared_ptr<RawSocket>& socket);
        void start_rx(CircularBuffer<uint8_t, DEFAULT_BUFFER_SIZE>& buffer);
        void stop_rx();

    private:
        std::atomic<bool> running_{ false };
        std::shared_ptr<RawSocket> rx_socket_;
        int32_t epoll_fd_{ -1 };
        epoll_event events_[1];
    };

} // namespace network
