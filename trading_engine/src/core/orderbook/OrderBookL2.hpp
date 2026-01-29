#pragma once

#include <utility>
#include <vector>
#include <cstdint>

#include "../network/message.hpp"

using Price = uint32_t;
using Qty = uint32_t;

class OrderbookStats;

class OrderBookL2
{
public:
    OrderBookL2();

    OrderBookL2(const OrderBookL2&) = delete;
    OrderBookL2& operator=(const OrderBookL2&) = delete;
    OrderBookL2(OrderBookL2&&) = default;
    OrderBookL2& operator=(OrderBookL2&&) = default;
    ~OrderBookL2() = default;
    
    void process_exchange_message(const Message& msg) noexcept;

private:
    std::vector<std::pair<Price, Qty>> bid_levels_;
    std::vector<std::pair<Price, Qty>> ask_levels_;

    void process_order(const uint8_t order_type, const uint32_t price, const uint32_t qty, const uint8_t side) noexcept;

    template <typename T, typename Compare>
    void process_order(const uint8_t msg_type, T& levels, const uint32_t price, const uint32_t qty, Compare comp) noexcept;

    friend class OrderbookStats;
};