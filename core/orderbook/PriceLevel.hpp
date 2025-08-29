#pragma once

#include <vector>

#include "Order.hpp"

#define INIT_LEVEL_SIZE 1024

class PriceLevel
{
public:
    PriceLevel(const Order& order)
    {
        price_ = order.get_price();
        orders_.reserve(INIT_LEVEL_SIZE);
        orders_.push_back(order);
    };

    PriceLevel(PriceLevel&& price_level)
    {
        price_ = price_level.get_price();
        orders_ = std::move(price_level.get_orders());
    }

    auto operator=(PriceLevel&& price_level) -> PriceLevel&
    {
        price_ = price_level.get_price();
        orders_ = std::move(price_level.get_orders());

        return *this;
    }

    PriceLevel(const PriceLevel&) = delete;
    void operator=(const PriceLevel&) = delete;

    ~PriceLevel() = default;

    auto get_price() const -> Price { return price_; }
    auto get_orders() -> std::vector<Order>& { return orders_; }
    
private:
    Price price_;
    std::vector<Order> orders_;
};

