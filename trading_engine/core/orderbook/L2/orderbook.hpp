#pragma once

#include <vector>
#include <utility>

#include "../orderbook_types.hpp"

#define MAX_LEVELS 1024

class OrderbookStats;

class OrderBook
{
public:
    OrderBook();

    OrderBook(const OrderBook&) = delete;
    OrderBook& operator=(const OrderBook&) = delete;
    OrderBook(OrderBook&&) = default;
    OrderBook& operator=(OrderBook&&) = default;
    ~OrderBook() = default;
    
    void process_order(const Action, const Price, const Volume, const Side) noexcept;

private:
    std::vector<std::pair<Price, Volume>> bid_levels_;
    std::vector<std::pair<Price, Volume>> ask_levels_;

    template <typename T, typename Compare>
    void process_order(Action action, T& levels, const Price price, const Volume volume, Compare comp) noexcept;

    friend class OrderbookStats;
};