#include <algorithm>
#include <print>
#include <memory>
#include <thread>
#include <chrono>

#include "orderbook/L2/orderbook.hpp"

int main()
{
    L2::OrderBook orderbook{};
    
    orderbook.add_order(Price{10}, Volume{50}, Side::Bid);
    orderbook.add_order(Price{10}, Volume{50}, Side::Bid);
    orderbook.add_order(Price{8}, Volume{50}, Side::Bid);
    orderbook.add_order(Price{15}, Volume{50}, Side::Bid);
    orderbook.add_order(Price{13}, Volume{50}, Side::Bid);
    orderbook.add_order(Price{9}, Volume{50}, Side::Bid);
    orderbook.add_order(Price{13}, Volume{50}, Side::Bid);
    orderbook.add_order(Price{19}, Volume{50}, Side::Bid);
    orderbook.add_order(Price{11}, Volume{50}, Side::Bid);
    orderbook.add_order(Price{14}, Volume{50}, Side::Bid);
    orderbook.add_order(Price{14}, Volume{50}, Side::Bid);

    for (auto& x : orderbook.bid_levels_) {
        std::print("Price: {}, Volume: {}\n", x.first, x.second);
    }

    return 0;
}   
