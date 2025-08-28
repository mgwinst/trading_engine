#include <ranges>
#include <utility>
#include <algorithm>

#include "OrderBook.hpp"

namespace rng = std::ranges;

OrderBook::OrderBook()
{

}

OrderBook::~OrderBook()
{

}

// reverse vector ordering to reduce copies
auto OrderBook::add_order(const Order& order) -> void 
{
    if (order.get_side() == Side::Bid)
        return add_order(order, bid_levels_, std::greater<Price>());
    else
        return add_order(order, ask_levels_, std::less<Price>());
}

template <typename T, typename Compare>
auto OrderBook::add_order(const Order& order, T& levels, Compare comp) -> void 
{
    auto levels_iter = rng::find_if(rng::reverse_view(levels), [order, comp](const PriceLevel& price_level) { return order.get_price() == price_level.get_price(); });

    if (levels_iter != levels.rend()) [[likely]] {
        levels_iter->get_orders().push_back(order);
    } else {
        auto new_price_pos = rng::find_if(rng::reverse_view(levels), [order, comp](const PriceLevel& price_level) { return comp(order.get_price(), price_level.get_price()); });
        levels.insert(new_price_pos.base(), PriceLevel{ order });
    }
}

auto OrderBook::cancel_order(const Order& order) -> void
{
    if (order.get_side() == Side::Bid)
        cancel_order(order, bid_levels_);
    else
        cancel_order(order, ask_levels_);

}

template <typename T>
auto OrderBook::cancel_order(const Order& order, T& levels) -> void
{
    auto price_iter = rng::find_if(levels, [order](const PriceLevel &price_level) { return price_level.get_price() == order.get_price(); });
    if (price_iter != levels.end()) {
        auto order_iter = rng::find_if(price_iter->get_orders(), [order](const Order &o) { return o.get_order_id() == order.get_order_id(); });
        if (order_iter != price_iter->get_orders().end())
            price_iter->get_orders().erase(order_iter);
    }
}

auto OrderBook::modify_order(const Order& order) -> void
{

}

// use in modify, check for volume and price changes (refer back to OB protocols)
auto OrderBook::cancel_and_replace(const Order& order) -> void 
{

}

// auto OrderBook::get_best_prices() {}