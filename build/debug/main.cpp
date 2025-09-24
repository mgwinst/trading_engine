#include <algorithm>
#include <print>
#include <memory>
#include <thread>
#include <chrono>

#include "common/queues/CircularBuffer.hpp"
#include "common/thread_utils.hpp"
#include "itch/msg_parser.hpp"
#include "orderbook/L2/orderbook.hpp"
#include "network/socket/raw_socket.hpp"
#include "network/socket/feed_handler.hpp"

int main()
{
    auto socket = std::make_shared<network::RawSocket>("eno1");

    CircularBuffer<uint8_t, DEFAULT_BUFFER_SIZE> rx_buffer;

    network::FeedHandler feed_handler{};
    feed_handler.add_to_epoll_list(socket);

    feed_handler.start_rx(rx_buffer);

    return 0;
}   
