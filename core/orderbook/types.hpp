#pragma once

#include <cstdint>
#include <cstddef>

struct ExchangeMessage 
{
    std::byte symbol[8];         
    std::byte padding[56];
};

enum class OrderType
{ 
    GTC,
    FOK
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