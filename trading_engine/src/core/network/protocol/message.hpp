#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "../../common/macros.hpp"
#include "../../common/concepts.hpp"

// https://nasdaqtrader.com/content/technicalSupport/specifications/dataproducts/binaryfile.pdf

struct itchmsg
{
    uint16_t len;
    uint8_t data[];
};

struct alignas(64) Message 
{
    uint64_t order_id;
    uint64_t timestamp;
    uint32_t price;
    uint32_t qty;
    uint16_t symbol_id;
    uint8_t  msg_type;
    uint8_t  side;
};

template <std::integral T>
[[nodiscard]] FORCE_INLINE T load_be(const std::byte* __restrict__ p) noexcept
{
    T value;
    std::memcpy(&value, p, sizeof(T));
    return std::byteswap(value);
}

// tags
struct Add {};
struct Cancel {};
struct Delete {};
struct Replace {};
struct Execute {};

template <typename T>
struct Offset 
{
    static_assert(always_false_v<T>, "Offset<T>: no specialization for type T");
};

template <>
struct Offset<Add>
{
    static constexpr std::size_t TYPE      = 0; // 'A'
    static constexpr std::size_t LOCATE    = 1;
    static constexpr std::size_t TIMESTAMP = 5;
    static constexpr std::size_t ORDER_ID  = 11;
    static constexpr std::size_t SIDE      = 19;
    static constexpr std::size_t QTY       = 20;
    static constexpr std::size_t PRICE     = 32;
};

template <>
struct Offset<Cancel>
{
    static constexpr std::size_t TYPE      = 0; // 'X'
    static constexpr std::size_t LOCATE    = 1;
    static constexpr std::size_t TIMESTAMP = 5;
    static constexpr std::size_t ORDER_ID  = 11;
    static constexpr std::size_t QTY       = 19;
};

template <>
struct Offset<Delete>
{
    static constexpr std::size_t TYPE      = 0; // 'D'
    static constexpr std::size_t LOCATE    = 1;
    static constexpr std::size_t TIMESTAMP = 5;
    static constexpr std::size_t ORDER_ID  = 11;
};

template <>
struct Offset<Replace>
{
    static constexpr std::size_t TYPE         = 0; // 'U'
    static constexpr std::size_t LOCATE       = 1;
    static constexpr std::size_t TIMESTAMP    = 5;
    static constexpr std::size_t OLD_ORDER_ID = 11;
    static constexpr std::size_t NEW_ORDER_ID = 19;
    static constexpr std::size_t QTY          = 27;
    static constexpr std::size_t PRICE        = 31;
};

template <>
struct Offset<Execute>
{
    static constexpr std::size_t TYPE         = 0; // 'E'
    static constexpr std::size_t LOCATE       = 1;
    static constexpr std::size_t TIMESTAMP    = 5;
    static constexpr std::size_t ORDER_ID     = 11;
    static constexpr std::size_t QTY          = 19;
};

template <typename T>
requires std::same_as<T, Add>
FORCE_INLINE void decode(const std::byte* __restrict__ p, Message& msg) noexcept
{
    msg.msg_type  = (uint8_t) p[Offset<Add>::TYPE];
    msg.symbol_id = load_be<uint16_t>(p + Offset<Add>::LOCATE);
    msg.order_id  = load_be<uint64_t>(p + Offset<Add>::ORDER_ID);
    msg.price     = load_be<uint32_t>(p + Offset<Add>::PRICE);
    msg.qty       = load_be<uint32_t>(p + Offset<Add>::QTY);
    msg.side      = (uint8_t) p[Offset<Add>::SIDE];
}

template <typename T>
requires std::same_as<T, Cancel>
FORCE_INLINE void decode(const std::byte* __restrict__ p, Message& msg) noexcept
{
    msg.msg_type  = (uint8_t) p[Offset<Cancel>::TYPE];
    msg.symbol_id = load_be<uint16_t>(p + Offset<Cancel>::LOCATE);
    msg.order_id  = load_be<uint64_t>(p + Offset<Cancel>::ORDER_ID);
    msg.qty       = load_be<uint32_t>(p + Offset<Cancel>::QTY);
}

template <typename T>
requires std::same_as<T, Delete>
FORCE_INLINE void decode(const std::byte* __restrict__ p, Message& msg) noexcept
{
    msg.msg_type  = (uint8_t) p[Offset<Delete>::TYPE];
    msg.symbol_id = load_be<uint16_t>(p + Offset<Delete>::LOCATE);
    msg.order_id  = load_be<uint64_t>(p + Offset<Delete>::ORDER_ID);
}

template <typename T>
requires std::same_as<T, Replace>
FORCE_INLINE void decode(const std::byte* __restrict__ p, Message& msg) noexcept
{
    msg.msg_type  = (uint8_t) p[Offset<Replace>::TYPE];
    msg.symbol_id = load_be<uint16_t>(p + Offset<Replace>::LOCATE);
    msg.order_id  = load_be<uint64_t>(p + Offset<Replace>::OLD_ORDER_ID); // need new ID too
    msg.price     = load_be<uint32_t>(p + Offset<Replace>::PRICE);
    msg.qty       = load_be<uint32_t>(p + Offset<Replace>::QTY);
}

template <typename T>
requires std::same_as<T, Execute>
FORCE_INLINE void decode(const std::byte* __restrict__ p, Message& msg) noexcept
{
    msg.msg_type = (uint8_t) p[Offset<Execute>::TYPE];
    msg.msg_type = load_be<uint16_t>(p + Offset<Execute>::LOCATE);
    msg.order_id = load_be<uint64_t>(p + Offset<Execute>::ORDER_ID);
    msg.qty      = load_be<uint32_t>(p + Offset<Execute>::QTY);
}

FORCE_INLINE __attribute__((flatten)) void process_msg(itchmsg* __restrict__ itch_msg, Message& msg) noexcept
{
    const std::byte* __restrict__ p = reinterpret_cast<const std::byte*>(itch_msg->data);

    switch (static_cast<char>(p[0])) {
        case 'A': decode<Add>(p, msg); break;
        case 'X': decode<Cancel>(p, msg); break;
        case 'D': decode<Delete>(p, msg); break;
        case 'U': decode<Replace>(p, msg); break;
        case 'E': decode<Execute>(p, msg); break;
        default: break;
    }
}

// hot staging a local tmp Message object is useful when we don't yet know if we need it...
// otherwise, just decode directly into the next open slot in the queue