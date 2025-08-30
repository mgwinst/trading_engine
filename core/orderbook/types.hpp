#pragma once

#include <cstdint>
#include <cstddef>

namespace orderbook
{
    struct ExchangeMessage 
    {
        std::byte symbol[8];         
        std::byte padding[56];
    };

    enum class OrderType
    { 
        Market,
        Limit
    };

    enum class Side
    { 
        Bid,
        Ask 
    };

    using OrderId  = uint64_t;
    using TickerId = uint64_t;
    using Price = uint64_t;
    using Volume = uint64_t;
    using Priority = uint64_t;

    struct BestPrice
    {
        Price bid;
        Price ask;
    };
} // end of namespace
