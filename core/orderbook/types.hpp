#pragma once

#include <cstdint>
#include <cstddef>

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

enum class Action
{
    Add,
    Cancel
};

using OrderId  = uint64_t;
using TickerId = uint64_t;
using Price = uint64_t;
using Volume = uint64_t;
using Priority = uint64_t;