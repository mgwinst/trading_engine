#include <print>
#include <algorithm>

#include "orderbook/OrderBook.hpp"

using namespace orderbook;

int main()
{
    OrderBook orderbook{};
 
    constexpr int num_orders = 10;

    std::array<Price, num_orders> bid_prices {8, 8, 4, 7, 3, 4, 8, 7, 6, 5};
    std::array<Volume, num_orders> bid_volumes {100, 50, 20, 25, 10, 10, 70, 60, 35, 50};

    std::array<Price, num_orders> ask_prices {8, 8, 9, 15, 12, 11, 10, 9, 13, 20};
    std::array<Volume, num_orders> ask_volumes {100, 50, 20, 25, 10, 10, 70, 60, 35, 50};

    for (size_t i{}; i < num_orders; i++) {
        orderbook.add_order(Order{i, 0, OrderType::Market, Side::Bid, bid_prices[i], bid_volumes[i]});
        orderbook.add_order(Order{i, 0, OrderType::Market, Side::Ask, ask_prices[i], ask_volumes[i]});
    }

    

    // std::print("Best bid: {}\nBest ask: {}\n", orderbook.best_prices().bid, orderbook.best_prices().ask);

}

