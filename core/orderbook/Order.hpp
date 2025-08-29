#pragma once

#include <orderbook/types.hpp>
#include <format>

class Order 
{
public:
    Order(OrderId order_id, TickerId ticker_id, OrderType order_type, Side side, Price price, Volume volume) : 
        order_id_{ order_id },
        ticker_id_{ ticker_id },
        order_type_{ order_type },
        side_{ side },
        price_{ price },
        volume_{ volume } 
    {}

    Order(const Order&) = default;
    Order& operator=(const Order&) = default;
    Order(Order&&) = default;
    Order& operator=(Order&&) = default;
    ~Order() = default;
    
    auto get_order_id() const noexcept -> OrderId { return order_id_; }
    auto get_ticker_id() const noexcept -> TickerId { return ticker_id_; }
    auto get_order_type() const noexcept -> OrderType { return order_type_; }
    auto get_side() const noexcept -> Side { return side_; }
    auto get_price() const noexcept -> Price { return price_; }
    auto get_volume() const noexcept -> Volume { return volume_; }
    auto to_string() const noexcept -> std::string
    {
        return std::format("<Order ID: [{}], Ticker: [{}], Type: [{}], Side: [{}], Price: [${}], Volume: [{}]>", 
            order_id_,
            ticker_id_,
            order_type_ == OrderType::GTC ? "GTC" : "FOK",
            side_ == Side::Bid ? "Bid" : "Ask",
            price_,
            volume_
        );
    }
    
private:
    OrderId order_id_;
    TickerId ticker_id_;
    OrderType order_type_;
    Side side_;
    Price price_;
    Volume volume_;
};