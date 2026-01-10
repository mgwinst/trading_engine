#include "../../core/common/json_parser.hpp"
#include "../../core/network/socket/feed_handler.hpp"
#include "../../core/common/macros.hpp"

int main()
{
    auto interface = parse_interface_from_json("../../config.json");

    if (!interface)
        error_exit("interface does not exist");

    auto feed_handler = network::FeedHandler(*interface);

    


}

/*
auto orderbook = OrderBook{};
auto stats = OrderbookStats{ orderbook };

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
*/