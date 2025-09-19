#pragma once

#include <type_traits>
#include <bit>

#include "itch/moldudp64.hpp"

void simd_parse_mold_packet(void *);
void parse_mold_packet(void *);

template <typename T>
void network_to_host(T& value)
{
    static_assert(false, "function not implemented for type T");
}

template <>
inline void network_to_host<AddOrderMessage>(AddOrderMessage& value)
{
    value.stock_locate = std::byteswap(value.stock_locate);
    value.tracking_number = std::byteswap(value.tracking_number);
    value.reference_number = std::byteswap(value.reference_number);
    value.num_shares = std::byteswap(value.num_shares);
    value.price = std::byteswap(value.price);
}

template <>
inline void network_to_host<OrderCancelMessage>(OrderCancelMessage& value)
{
    value.stock_locate = std::byteswap(value.stock_locate);
    value.tracking_number = std::byteswap(value.tracking_number);
    value.reference_number = std::byteswap(value.reference_number);
    value.num_cancelled_shares = std::byteswap(value.num_cancelled_shares);
}

template <>
inline void network_to_host<OrderDeleteMessage>(OrderDeleteMessage& value)
{
    value.stock_locate = std::byteswap(value.stock_locate);
    value.tracking_number = std::byteswap(value.tracking_number);
    value.reference_number = std::byteswap(value.reference_number);
}

template <>
inline void network_to_host<OrderReplaceMessage>(OrderReplaceMessage& value)
{
    value.stock_locate = std::byteswap(value.stock_locate);
    value.tracking_number = std::byteswap(value.tracking_number);
    value.original_reference_number = std::byteswap(value.original_reference_number);
    value.new_reference_number = std::byteswap(value.new_reference_number);
    value.num_shares = std::byteswap(value.num_shares);
    value.price = std::byteswap(value.price);
}

template <typename T>
requires std::is_trivial_v<T>
T deserialize(msgblk* msg)
{
    alignas(T) std::array<std::byte, sizeof(T)> buffer;
    std::memcpy(buffer.data(), msg->data, sizeof(T));
    auto value = std::bit_cast<T>(buffer);

    network_to_host(value);

    return value;
}