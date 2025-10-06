#include <unordered_map>
#include <algorithm>
#include <print>
#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include <array>

#include "common/queues/MessageQueues.hpp"
#include "common/thread_utils.hpp"
#include "common/json_parser.hpp"
#include "network/socket/raw_socket.hpp"
#include "network/socket/feed_handler.hpp"
#include "orderbook/orderbook_manager.hpp"

struct ExchangeMessage
{
    const char* value;
};

int main()
{
    auto iface = parse_interface_from_json("../../config.json");   
    auto tickers = parse_tickers_from_json("../../config.json");

    auto feed_handler = network::make_feed_handler(*iface);

    auto books = OrderbookManager{*tickers};
    auto& msg_queues = MessageQueues<ExchangeMessage>::get_instance();

    ExchangeMessage msg1{"[NVDA] $50"};
    ExchangeMessage msg2{"[TSLA] $25"};
    ExchangeMessage msg3{"[PLTR] $300"};

    msg_queues["NVDA"]->try_push(msg1);
    msg_queues["TSLA"]->try_push(msg2);
    msg_queues["PLTR"]->try_push(msg3);

    auto a = msg_queues["NVDA"]->try_pop();
    auto b = msg_queues["TSLA"]->try_pop();
    auto c = msg_queues["PLTR"]->try_pop();

    std::println("{}", a.value);
    std::println("{}", b.value);
    std::println("{}", c.value);

}   
