#include <algorithm>
#include <print>
#include <memory>
#include <thread>
#include <chrono>

#include "common/thread_utils.hpp"
#include "orderbook/L2/orderbook.hpp"
#include "network/socket/raw_socket.hpp"
#include "network/socket/feed_handler.hpp"

using namespace std::chrono_literals;

int main()
{
    auto fh = network::init_feed_handler("eno1");

    std::this_thread::sleep_for(5s);

}   
