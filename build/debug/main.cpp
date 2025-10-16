#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <print>
#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include <array>
#include <cassert>

/*
#include "common/thread_utils.hpp"
#include "common/CoreSet.hpp"
#include "network/socket/raw_socket.hpp"
#include "network/socket/feed_handler.hpp"
#include "orderbook/orderbook_manager.hpp"
#include "parser/msg_types.hpp"
#include "common/time_utils.hpp"
*/

#include "common/queues/MessageQueues.hpp"
#include "parser/deserialize.hpp"
#include "parser/msg_types.hpp"
#include "parser/BinaryFILE.hpp"
#include "parser/msg_parser.hpp"
#include "common/json_parser.hpp"
#include "orderbook/L2/orderbook.hpp"
#include "stats/OrderbookStats.hpp"

int main()
{
    auto orderbook = OrderBook{};

    orderbook.process_order(Action::Add, 10, 25, 'B');
    orderbook.process_order(Action::Add, 15, 50, 'B');
    orderbook.process_order(Action::Add, 8, 100, 'B');
    orderbook.process_order(Action::Add, 9, 100, 'B');
    orderbook.process_order(Action::Add, 10, 75, 'B');
    orderbook.process_order(Action::Add, 10, 20, 'B');
    orderbook.process_order(Action::Add, 9, 250, 'B');
    orderbook.process_order(Action::Add, 13, 10, 'B');
    orderbook.process_order(Action::Add, 11, 10, 'B');
    orderbook.process_order(Action::Add, 11, 80, 'B');

    auto stats = OrderbookStats{ orderbook };

}   
