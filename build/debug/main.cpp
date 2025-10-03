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

};

int main()
{
    auto interface = parse_interface_from_json("../../config.json");   
    auto tickers = parse_tickers_from_json("../../config.json");

    auto feed_handler = network::make_feed_handler(*interface);

    auto& msg_queues = MessageQueues<ExchangeMessage>::get_instance();

    msg_queues.add_queues(*tickers);
    

}   
