#pragma once

#include <cstdint>

struct ExchangeMessage 
{
    std::byte symbol[8];         
    std::byte padding[56];
};

enum class OrderType : uint8_t 
{ 
    FillOrKill,
    GoodTillCancel 
};

enum class Side : uint8_t 
{ 
    Bid,
    Ask 
};

using OrderId  = uint64_t;
using TickerId = uint64_t;
using Price = uint64_t;
using Volume = uint64_t;
using Priority = uint64_t;

struct Order 
{
    OrderId order_id_;
    TickerId ticker_id_;
    OrderType order_type_;
    Side side_;
    Price price_;
    Volume volume_;

    Order(OrderId order_id, TickerId ticker_id, OrderType order_type, Side side, Price price, Volume volume) : 
        order_id_{ order_id },
        ticker_id_{ ticker_id },
        order_type_{ order_type },
        side_{ side },
        price_{ price },
        volume_{ volume } {}

};

struct OrderEntry
{
    OrderId order_id_;
    Side side_;
};

struct PriceLevel
{
    Price price_;
    std::vector<Order> orders_;
};