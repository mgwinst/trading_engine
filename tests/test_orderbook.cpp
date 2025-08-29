#include <gtest/gtest.h>
#include <algorithm>
#include <print>

#include "orderbook/OrderBook.hpp"

TEST(OrderBook, Ordering) { 
    OrderBook orderbook{};
 
    std::array<int, 10> bid_prices {8, 8, 4, 7, 3, 4, 8, 7, 6, 5};
    std::array<int, 10> bid_volumes {100, 50, 20, 25, 10, 10, 70, 60, 35, 50};

    std::array<int, 10> ask_prices {8, 8, 9, 15, 12, 11, 10, 9, 13, 20};
    std::array<int, 10> ask_volumes {100, 50, 20, 25, 10, 10, 70, 60, 35, 50};

    for (size_t i{}; i < 10; i++) {
        orderbook.add_order(Order{i, 0, OrderType::GTC, Side::Bid, bid_prices[i], bid_volumes[i]});
        orderbook.add_order(Order{i, 0, OrderType::GTC, Side::Ask, ask_prices[i], ask_volumes[i]});
    }

    /*

    std::ranges::for_each(orderbook.bid_levels_, 
        [](PriceLevel& price_level) { for (auto x : price_level.get_orders()) { std::print("{}\n", x.to_string()); }});

    std::println();

    std::ranges::for_each(orderbook.ask_levels_, 
        [](PriceLevel& price_level) { for (auto x : price_level.get_orders()) { std::print("{}\n", x.to_string()); }});   

    */
}
