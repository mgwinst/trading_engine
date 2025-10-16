#pragma once

#include <vector>
#include <optional>

#include "orderbook/orderbook_types.hpp"
#include "orderbook/L2/orderbook.hpp"

class OrderbookStats
{
public:
    OrderbookStats(const OrderBook& orderbook) : orderbook_{ orderbook } {}

    std::vector<std::pair<Price, Volume>> bid_prices(std::optional<std::size_t> count = std::nullopt) const noexcept;
    std::vector<std::pair<Price, Volume>> ask_prices(std::optional<std::size_t> count = std::nullopt) const noexcept;
    std::pair<Price, Price> best_prices() const noexcept;
    Price best_bid() const noexcept;       
    Price best_ask() const noexcept;       
    Price mid_price() const noexcept;
    int32_t bid_depth(std::optional<std::size_t> count = std::nullopt) const noexcept;
    int32_t ask_depth(std::optional<std::size_t> count = std::nullopt) const noexcept;
    double imbalance(std::optional<std::size_t> count = std::nullopt) const noexcept;
    int32_t absolute_spread() const noexcept;
    double relative_spread() const noexcept;

private:
    const OrderBook& orderbook_;

    template <typename T>
    std::vector<std::pair<Price, Volume>> get_prices(T& levels, std::optional<std::size_t> count) const noexcept;

    template <typename T>
    int32_t get_market_depth(T& levels, std::optional<std::size_t> count) const noexcept;
};