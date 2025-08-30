#pragma once

#include <vector>

#include "Order.hpp"

#define INIT_LEVEL_SIZE 1024

namespace orderbook
{
    class PriceLevel
    {
    public:
        PriceLevel(const Order& order) noexcept
        {
            price_ = order.price();
            orders_.reserve(INIT_LEVEL_SIZE);
            orders_.push_back(order);
        };

        PriceLevel(PriceLevel&& price_level) noexcept
        {
            price_ = price_level.price();
            orders_ = std::move(price_level.orders());
        }

        auto operator=(PriceLevel&& price_level) noexcept -> PriceLevel&
        {
            price_ = price_level.price();
            orders_ = std::move(price_level.orders());

            return *this;
        }

        PriceLevel(const PriceLevel&) = delete;
        void operator=(const PriceLevel&) = delete;
        ~PriceLevel() = default;

        auto price() const noexcept -> Price { return price_; }
        auto orders() noexcept -> std::vector<Order>& { return orders_; }
        
    private:
        Price price_;
        std::vector<Order> orders_;
    };

} // end of namespace
