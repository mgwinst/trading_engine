#include <algorithm>
#include <print>
#include <memory>
#include <thread>
#include <chrono>

#include "common/CircularBuffer.hpp"
#include "common/thread_utils.hpp"
#include "orderbook/L2/orderbook.hpp"
#include "network/socket/raw_socket.hpp"
#include "network/socket/feed_handler.hpp"

using namespace std::chrono_literals;

int main()
{
    auto socket = std::make_shared<network::RawSocket>("eno1");
    auto feed_handler = std::make_shared<network::FeedHandler>(socket);

    return 0;
}   
