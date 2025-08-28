#pragma once

#include <orderbook/types.hpp>

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
    
    auto get_order_id() const noexcept -> OrderId { return order_id_; }
    auto get_ticker_id() const noexcept -> TickerId { return ticker_id_; }
    auto get_order_type() const noexcept -> OrderType { return order_type_; }
    auto get_side() const noexcept -> Side { return side_; }
    auto get_price() const noexcept -> Price { return price_; }
    auto get_volume() const noexcept -> Volume { return volume_; }
    
private:
    OrderId order_id_;
    TickerId ticker_id_;
    OrderType order_type_;
    Side side_;
    Price price_;
    Volume volume_;
};