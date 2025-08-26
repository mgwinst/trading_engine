#pragma once

#include <vector>

#include "order.hpp"

namespace engine {

    struct OrderBook {
        std::vector<Order> bid_levels_;
        std::vector<Order> ask_levels_;

        auto add_order(const Order& order) -> void;

        template <typename T, typename Compare>
        auto add_order(OrderId order_id, T& levels, Price price, Volume volume, Compare comp) -> void;

        auto modify_order(const Order& order) -> void;
        auto cancel_order(const Order& order) -> void;
        auto get_best_prices() const -> std::pair<Price, Volume>;
    };

}