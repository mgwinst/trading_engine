#include <algorithm>
#include <print>
#include <memory>
#include <thread>
#include <chrono>

#include "orderbook/L2/orderbook.hpp"

int main()
{
    L2::OrderBook orderbook{};
    
    orderbook.process_order(Action::Add, Price{10}, Volume{50}, Side::Bid);
    orderbook.process_order(Action::Add, Price{11}, Volume{50}, Side::Bid);
    orderbook.process_order(Action::Add, Price{12}, Volume{50}, Side::Bid);

    orderbook.process_order(Action::Cancel, Price{12}, Volume{20}, Side::Bid);
    orderbook.process_order(Action::Cancel, Price{12}, Volume{30}, Side::Bid);

    for (const auto x : orderbook.bid_levels_) {
        auto [price, volume] = x;
        std::print("Price: {} Volume: {}\n", price, volume);
    }

    return 0;
}   
