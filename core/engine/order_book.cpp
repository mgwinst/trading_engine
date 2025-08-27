#include <ranges>
#include <utility>
#include <algorithm>

#include "order_book.hpp"

namespace engine 
{
    // rethink this... start up is going to be slow b/c we need to introduce new price levels as orders come in. We can't anticipate where the price will begin. Can we?
    OrderBook::OrderBook()
    {
        bid_levels_.resize(32);
        std::ranges::for_each(bid_levels_, [](PriceLevel price_level) { price_level.orders_.reserve(1024); });

        ask_levels_.resize(32);
        std::ranges::for_each(ask_levels_, [](PriceLevel price_level) { price_level.orders_.reserve(1024); });
    }

    OrderBook::~OrderBook()
    {

    }

    // reverse vector ordering to reduce copies
    auto OrderBook::add_order(const Order& order) -> void 
    {
        if (order.side_ == Side::Bid)
            return add_order(order, bid_levels_, std::equal_to<Price>());
        else
            return add_order(order, ask_levels_, std::equal_to<Price>());
    }

    template <typename T, typename Compare>
    auto OrderBook::add_order(const Order& order, T& levels, Compare comp) -> void 
    {
        auto it = std::ranges::find_if(levels, [order, comp](const PriceLevel& price_level) { return comp(price_level.price_, order.price_); });

        if (it != levels.end()) 
            it->orders_.push_back(order);
    }

    auto OrderBook::cancel_order(const Order& order) -> void
    {
        if (order.side_ == Side::Bid)
            cancel_order(order, bid_levels_);
        else
            cancel_order(order, ask_levels_);

    }

    template <typename T>
    auto OrderBook::cancel_order(const Order& order, T& levels) -> void
    {
        auto price_iter = std::ranges::find_if(levels, [order](const PriceLevel& price_level) { return price_level.price_ == order.price_; });
        auto order_iter = std::ranges::find_if(price_iter->orders_, [order](const Order& o) { return o.order_id_ == order.order_id_; });

        if (order_iter != price_iter->orders_.end())
            price_iter->orders_.erase(order_iter);
    }

    // use in modify, check for volume and price changes (refer back to OB protocols)
    auto OrderBook::cancel_and_replace(const Order& order) -> void 
    {

    }

    // auto OrderBook::get_best_prices() {}

}
