#pragma once

#include "../common/bytes.hpp"
#include "../common/concepts.hpp"
#include "../orderbook/symbol_directory.hpp"

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

struct Add {};
struct Cancel {};
struct Delete {};
struct Replace {};
struct Execute {};
struct StockDirectory{};
struct SystemEvent{};

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
    static constexpr std::size_t TYPE      = 0; // 'E'
    static constexpr std::size_t LOCATE    = 1;
    static constexpr std::size_t TIMESTAMP = 5;
    static constexpr std::size_t ORDER_ID  = 11;
    static constexpr std::size_t QTY       = 19;
};

template <>
struct Offset<StockDirectory>
{
    static constexpr std::size_t TYPE       = 0; // 'R'
    static constexpr std::size_t LOCATE     = 1;
    static constexpr std::size_t TIMESTAMP  = 5;
    static constexpr std::size_t SYMBOL_STR = 11; // -> uint8_t[8] 8 byte alpha buffer, right padded
};

template <typename T>
struct Decoder
{
    static_assert(always_false_v<T>, "Decoder<T>: no specialization for type T");
};

template <>
struct Decoder<Add>
{
    using T = Add;

    static FORCE_INLINE void decode(const std::byte* __restrict__ p, Message& msg) noexcept
    {
        msg.msg_type  = load_byte(p + Offset<T>::TYPE);
        msg.symbol_id = load_be<uint16_t>(p + Offset<T>::LOCATE);
        msg.order_id  = load_be<uint64_t>(p + Offset<T>::ORDER_ID);
        msg.price     = load_be<uint32_t>(p + Offset<T>::PRICE);
        msg.qty       = load_be<uint32_t>(p + Offset<T>::QTY);
        msg.side      = load_byte(p + Offset<T>::SIDE);
    }
};

template <>
struct Decoder<Cancel>
{
    using T = Cancel;

    static FORCE_INLINE void decode(const std::byte* __restrict__ p, Message& msg) noexcept
    {
        msg.msg_type  = load_byte(p + Offset<T>::TYPE);
        msg.symbol_id = load_be<uint16_t>(p + Offset<T>::LOCATE);
        msg.order_id  = load_be<uint64_t>(p + Offset<T>::ORDER_ID);
        msg.qty       = load_be<uint32_t>(p + Offset<T>::QTY);
    }
};

template <>
struct Decoder<Delete>
{
    using T = Delete;

    static FORCE_INLINE void decode(const std::byte* __restrict__ p, Message& msg) noexcept
    {
        msg.msg_type  = load_byte(p + Offset<T>::TYPE);
        msg.symbol_id = load_be<uint16_t>(p + Offset<T>::LOCATE);
        msg.order_id  = load_be<uint64_t>(p + Offset<T>::ORDER_ID);
    }
};

template <>
struct Decoder<Replace>
{
    using T = Replace;

    static FORCE_INLINE void decode(const std::byte* __restrict__ p, Message& msg) noexcept
    {
        msg.msg_type  = load_byte(p + Offset<T>::TYPE);
        msg.symbol_id = load_be<uint16_t>(p + Offset<T>::LOCATE);
        msg.order_id  = load_be<uint64_t>(p + Offset<T>::OLD_ORDER_ID); // need new ID too
        msg.price     = load_be<uint32_t>(p + Offset<T>::PRICE);
        msg.qty       = load_be<uint32_t>(p + Offset<T>::QTY);
    }
};

template <>
struct Decoder<Execute>
{
    using T = Execute;

    static FORCE_INLINE void decode(const std::byte* __restrict__ p, Message& msg) noexcept
    {
        msg.msg_type  = load_byte(p + Offset<T>::TYPE);
        msg.symbol_id = load_be<uint16_t>(p + Offset<T>::LOCATE);
        msg.order_id  = load_be<uint64_t>(p + Offset<T>::ORDER_ID);
        msg.qty       = load_be<uint32_t>(p + Offset<T>::QTY);
    }
};

template <>
struct Decoder<StockDirectory> {};

template <>
struct Decoder<SystemEvent> {};
