#include <ranges>
#include <utility>
#include <algorithm>

#include "OrderBook.hpp"

namespace rng = std::ranges;

namespace orderbook
{
    OrderBook::OrderBook()
    {

    }

    OrderBook::~OrderBook()
    {

    }

    // reverse vector ordering to reduce copies
    auto OrderBook::add_order(const Order& order) noexcept -> void 
    {
        if (order.side() == Side::Bid)
            return add_order(order, bid_levels_, std::greater<Price>());
        else
            return add_order(order, ask_levels_, std::less<Price>());
    }

    template <typename T, typename Compare>
    auto OrderBook::add_order(const Order& order, T& levels, Compare comp) noexcept -> void 
    {
        auto level_iter = rng::find_if(rng::reverse_view(levels), [order, comp](const PriceLevel& price_level) { return order.price() == price_level.price(); });

        if (level_iter != levels.rend()) [[likely]] {
            level_iter->orders().push_back(order);
        } else {
            auto new_price_pos = rng::find_if(rng::reverse_view(levels), [order, comp](const PriceLevel& price_level) { return comp(order.price(), price_level.price()); });
            levels.insert(new_price_pos.base(), PriceLevel{ order });
        }
    }

    auto OrderBook::cancel_order(const Order& order) noexcept -> void
    {
        if (order.side() == Side::Bid)
            cancel_order(order, bid_levels_);
        else
            cancel_order(order, ask_levels_);
    }

    template <typename T>
    auto OrderBook::cancel_order(const Order& order, T& levels) noexcept -> void
    {
        auto level_iter = rng::find_if(rng::reverse_view(levels), [order](const PriceLevel& price_level) { return order.price() == price_level.price(); });

        if (level_iter != levels.rend()) [[likely]] {
            auto order_iter = rng::find_if(level_iter->orders(), [order](const Order &o) { return order.order_id() == o.order_id(); });
            if (order_iter != level_iter->orders().end()) [[likely]] {
                level_iter->orders().erase(order_iter);
                if (level_iter->orders().empty()) [[unlikely]] {
                    levels.erase(level_iter.base() - 1); // .base() returns fwd_iter + 1 position
                }
            }
        }
    }

    auto OrderBook::modify_order(const Order& order) noexcept -> void
    {
        if (order.side() == Side::Bid)
            modify_order(order, bid_levels_);
        else
            modify_order(order, ask_levels_);
    }

    template <typename T>
    auto OrderBook::modify_order(const Order& order, T& levels) noexcept -> void
    {
        
    }

    auto OrderBook::cancel_and_replace(const Order& order) noexcept -> void 
    {
        
    }

    auto OrderBook::best_prices() const noexcept -> BestPrice
    {
        return BestPrice{ bid_levels_.back().price(), ask_levels_.back().price() };
    }
} // end of namespace
