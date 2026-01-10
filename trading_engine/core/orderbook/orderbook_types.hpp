#pragma once

#include <cstdint>

enum class OrderType
{ 
    Market,
    Limit
};


enum class Action
{
    Add,
    Cancel
};

using Side = uint8_t;

using OrderId  = uint64_t;
using TickerId = uint64_t;
using Price = uint32_t;
using Volume = uint32_t;
// using Priority = uint64_t;