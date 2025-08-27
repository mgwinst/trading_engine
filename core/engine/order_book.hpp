#pragma once

#include <vector>

#include "order.hpp"

namespace engine 
{
    class OrderBook 
    {
        auto add_order(const Order& order) -> void;
        auto modify_order(const Order& order) -> void;
        auto cancel_order(const Order& order) -> void;
        auto get_best_prices() const -> void;

        OrderBook();
        OrderBook(const OrderBook&) = delete;
        void operator=(const OrderBook&) = delete;
        OrderBook(OrderBook&&) = delete;
        void operator=(OrderBook&&) = delete;
        ~OrderBook();

    private:
        std::vector<PriceLevel> bid_levels_;
        std::vector<PriceLevel> ask_levels_;

        template <typename T, typename Compare>
        auto add_order(const Order& order, T& levels, Compare comp) -> void;

        template <typename T>
        auto cancel_order(const Order& order, T& levels) -> void;

        auto cancel_and_replace(const Order& order) -> void;
        
    };

}