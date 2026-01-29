#pragma once

#include <concepts>

class OrderBookL2;
class OrderBookL3;

template <typename T>
concept OrderBookType = std::same_as<T, OrderBookL2> || std::same_as<T, OrderBookL3> 
    && requires (T t) {
        t.process_exchange_message();
    };
