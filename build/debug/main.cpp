#include <iostream>
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
#include "common/CoreSet.hpp"
#include "network/socket/raw_socket.hpp"
#include "network/socket/feed_handler.hpp"
#include "orderbook/orderbook_manager.hpp"
#include "parser/msg_types.hpp"

int main()
{
    std::filesystem::path config_path = std::getenv("TRADING_ENGINE_HOME");
    config_path /= "config.json";
    if (config_path.empty())
        throw std::runtime_error{ "config.json file not found" };
    
    auto interface = parse_interface_from_json(config_path.string());
    auto tickers = parse_tickers_from_json(config_path.string());   

    auto socket = std::make_shared<network::RawSocket>(*interface);
    auto feed_handler = std::make_shared<network::FeedHandler>(socket);

    // feed_handler->start_rx();
    // feed_handler->stop_rx();

    auto orderbooks = std::make_shared<OrderbookManager>(*tickers);

}   
