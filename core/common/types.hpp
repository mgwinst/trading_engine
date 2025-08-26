#pragma once

#include <cstdint>

namespace common {

    enum class OrderType : uint8_t { FillOrKill, GoodTillCancel };
    enum class Side : uint8_t { bid, ask };

    using OrderId  = uint64_t;
    using TickerId = uint64_t;
    using Price = uint64_t;
    using Volume = uint64_t;
    using Priority = uint64_t;

    alignas(64) struct ExchangeMessage
    {
        std::byte symbol[8];         
        std::byte padding[56];
    };

    

}
