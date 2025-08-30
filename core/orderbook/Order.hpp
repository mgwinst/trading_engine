#pragma once

#include <orderbook/types.hpp>
#include <format>

namespace orderbook
{
    class Order 
    {
    public:
        Order(OrderId order_id, TickerId ticker_id, OrderType order_type, Side side, Price price, Volume volume) : 
            order_id_{ order_id },
            ticker_id_{ ticker_id },
            order_type_{ order_type },
            side_{ side },
            price_{ price },
            initial_volume_{ volume },
            remaining_volume_{ volume } 
        {}

        Order(const Order&) = default;
        Order& operator=(const Order&) = default;
        Order(Order&&) = default;
        Order& operator=(Order&&) = default;
        ~Order() = default;
        
        auto order_id() const noexcept -> OrderId { return order_id_; }
        auto ticker_id() const noexcept -> TickerId { return ticker_id_; }
        auto order_type() const noexcept -> OrderType { return order_type_; }
        auto side() const noexcept -> Side { return side_; }
        auto price() const noexcept -> Price { return price_; }
        auto init_volume() const noexcept -> Volume { return initial_volume_; }
        auto remain_volume() const noexcept -> Volume { return remaining_volume_; }
        auto to_string() const noexcept -> std::string
        {
            return std::format("<order id: [{}], ticker: [{}], type: [{}], side: [{}], price: [${}], initial volume: [{}], remaining volume: [{}]>", 
                order_id_,
                ticker_id_,
                order_type_ == OrderType::Market ? "Market" : "Limit",
                side_ == Side::Bid ? "Bid" : "Ask",
                price_,
                initial_volume_,
                remaining_volume_
            );
        }
        
    private:
        OrderId order_id_;
        TickerId ticker_id_;
        OrderType order_type_;
        Side side_;
        Price price_;
        Volume initial_volume_;
        Volume remaining_volume_;
    };
} // end of namespace
