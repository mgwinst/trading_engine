#pragma once

#include <cstdint>

namespace common {

    enum class Side : uint8_t { bid, ask };

    using Order_ID  = uint64_t;
    using Ticker_ID = uint64_t;
    using Price = uint64_t;
    using Volume = uint64_t;
    using Priority = uint64_t;

    alignas(64) struct ExchangeMessage
    {
        char symbol[8];         
        char padding[56];
    };

}
