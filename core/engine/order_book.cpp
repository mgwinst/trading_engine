#include <ranges>
#include <utility>

#include "order_book.hpp"

// reverse vector ordering to reduce copies
auto engine::OrderBook::add_order(const Order& order) -> void {
    if (order.side_ == Side::bid)
        return add_order(order.order_id_, bid_levels_, order.price_, order.volume_, std::less<Price>());
    else
        return add_order(order.order_id_, ask_levels_, order.price_, order.volume_, std::greater<Price>());
}

template <typename T, typename Compare>
auto engine::OrderBook::add_order(OrderId order_id, T& levels, Price price, Volume volume, Compare comp) -> void {
    auto it = std::ranges::find_if(levels, price,
        [comp](const Order& order, Price price) { return comp(order.price_, price); });
    
    if (it != levels.end() && it->price_ == price) // [[attribute]]
        it->second += volume;
    else
        levels.insert(it, {price, volume});
}

auto engine::OrderBook::get_best_prices() {}