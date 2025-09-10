#pragma once

#include <vector>
#include <utility>

#include "types.hpp"
#include "PriceLevel.hpp"

namespace orderbook
{
    class OrderBook
    {
    public:
        OrderBook();
        OrderBook(const OrderBook&) = delete;
        void operator=(const OrderBook&) = delete;
        OrderBook(OrderBook&&) = delete;
        void operator=(OrderBook&&) = delete;
        ~OrderBook();

        auto add_order(const Order& order) noexcept -> void;
        auto cancel_order(const Order& order) noexcept -> void;
        auto modify_order(const Order& order) noexcept -> void;
        auto best_prices() const noexcept -> BestPrice;

    private:
        std::vector<PriceLevel> bid_levels_;
        std::vector<PriceLevel> ask_levels_;

        template <typename T, typename Compare>
        auto add_order(const Order& order, T& levels, Compare comp) noexcept -> void;

        template <typename T>
        auto cancel_order(const Order& order, T& levels) noexcept -> void;

        template <typename T>
        auto modify_order(const Order& order, T& levels) noexcept -> void;

        auto cancel_and_replace(const Order& order) noexcept -> void;
    };
} // end of namespace
