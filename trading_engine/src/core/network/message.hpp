#pragma once

#include "../common/macros.hpp"
#include "../common/bytes.hpp"
#include "../common/concepts.hpp"

struct moldhdr
{
    uint8_t  session[10];
    uint64_t seq_num;
    uint16_t msg_count;
    uint8_t payload[];
} PACKED;

struct moldmsg
{
    uint16_t len;
    uint8_t data[];
} PACKED;

// uint64_t symbol for stock directory messages ()
struct alignas(64) Message 
{
    uint64_t symbol; 
    uint64_t order_id;
    uint64_t new_order_id;
    uint64_t timestamp;
    uint32_t price;
    uint32_t qty;
    uint16_t locate;
    uint8_t msg_type;
    uint8_t side;
};

struct Add {};
struct Cancel {};
struct Delete {};
struct Replace {};
struct Execute {};
struct ExecutePrice {};
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
struct Offset<ExecutePrice>
{
    static constexpr std::size_t TYPE       = 0; // 'C'
    static constexpr std::size_t LOCATE     = 1;
    static constexpr std::size_t TIMESTAMP  = 5;
    static constexpr std::size_t ORDER_ID   = 11;
    static constexpr std::size_t QTY        = 19;
    static constexpr std::size_t MATCH_NUM  = 23;
    static constexpr std::size_t PRINTABLE  = 31;
    static constexpr std::size_t EXEC_PRICE = 32;
};

template <>
struct Offset<StockDirectory>
{
    static constexpr std::size_t TYPE       = 0; // 'R'
    static constexpr std::size_t LOCATE     = 1;
    static constexpr std::size_t TIMESTAMP  = 5;
    static constexpr std::size_t SYMBOL     = 11; // -> uint8_t[8] 8 byte alpha buffer, right padded
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

    static FORCE_INLINE Message decode(const std::byte* __restrict__ p) noexcept
    {
        Message msg{};

        msg.msg_type = static_cast<uint8_t>(*(p + Offset<T>::TYPE));
        msg.locate   = loadu_bswap<uint16_t>(p + Offset<T>::LOCATE);
        msg.order_id = loadu_bswap<uint64_t>(p + Offset<T>::ORDER_ID);
        msg.price    = loadu_bswap<uint32_t>(p + Offset<T>::PRICE);
        msg.qty      = loadu_bswap<uint32_t>(p + Offset<T>::QTY);
        msg.side     = static_cast<uint8_t>(*(p + Offset<T>::SIDE));

        return msg;
    }
};

template <>
struct Decoder<Cancel>
{
    using T = Cancel;

    static FORCE_INLINE Message decode(const std::byte* __restrict__ p) noexcept
    {
        Message msg{};
        
        msg.msg_type = static_cast<uint8_t>(*(p + Offset<T>::TYPE));
        msg.locate   = loadu_bswap<uint16_t>(p + Offset<T>::LOCATE);
        msg.order_id = loadu_bswap<uint64_t>(p + Offset<T>::ORDER_ID);
        msg.qty      = loadu_bswap<uint32_t>(p + Offset<T>::QTY);

        return msg;
    }
};

template <>
struct Decoder<Delete>
{
    using T = Delete;

    static FORCE_INLINE Message decode(const std::byte* __restrict__ p) noexcept
    {
        Message msg{};

        msg.msg_type = static_cast<uint8_t>(*(p + Offset<T>::TYPE));
        msg.locate   = loadu_bswap<uint16_t>(p + Offset<T>::LOCATE);
        msg.order_id = loadu_bswap<uint64_t>(p + Offset<T>::ORDER_ID);

        return msg;
    }
};

template <>
struct Decoder<Replace>
{
    using T = Replace;

    static FORCE_INLINE Message decode(const std::byte* __restrict__ p) noexcept
    {
        Message msg{};

        msg.msg_type     = static_cast<uint8_t>(*(p + Offset<T>::TYPE));
        msg.locate       = loadu_bswap<uint16_t>(p + Offset<T>::LOCATE);
        msg.order_id     = loadu_bswap<uint64_t>(p + Offset<T>::OLD_ORDER_ID);
        msg.new_order_id = loadu_bswap<uint64_t>(p + Offset<T>::NEW_ORDER_ID);
        msg.price        = loadu_bswap<uint32_t>(p + Offset<T>::PRICE);
        msg.qty          = loadu_bswap<uint32_t>(p + Offset<T>::QTY);

        return msg;
    }
};

template <>
struct Decoder<Execute>
{
    using T = Execute;

    static FORCE_INLINE Message decode(const std::byte* __restrict__ p) noexcept
    {
        Message msg{};

        msg.msg_type = static_cast<uint8_t>(*(p + Offset<T>::TYPE));
        msg.locate   = loadu_bswap<uint16_t>(p + Offset<T>::LOCATE);
        msg.order_id = loadu_bswap<uint64_t>(p + Offset<T>::ORDER_ID);
        msg.qty      = loadu_bswap<uint32_t>(p + Offset<T>::QTY);

        return msg;
    }
};

template <>
struct Decoder<ExecutePrice>
{
    using T = ExecutePrice;

    static FORCE_INLINE Message decode(const std::byte* __restrict__ p) noexcept
    {
        Message msg{};

        msg.msg_type = static_cast<uint8_t>(*(p + Offset<T>::TYPE));
        msg.locate   = loadu_bswap<uint16_t>(p + Offset<T>::LOCATE);
        msg.order_id = loadu_bswap<uint64_t>(p + Offset<T>::ORDER_ID);
        msg.qty      = loadu_bswap<uint32_t>(p + Offset<T>::QTY);
        msg.price    = loadu_bswap<uint32_t>(p + Offset<T>::EXEC_PRICE);

        return msg;
    }
};

static_assert(Offset<Execute>::ORDER_ID == Offset<ExecutePrice>::ORDER_ID);
static_assert(Offset<Execute>::QTY == Offset<ExecutePrice>::QTY);

template <>
struct Decoder<StockDirectory> {

    using T = StockDirectory;

    static FORCE_INLINE Message decode(const std::byte* __restrict__ p) noexcept
    {
        Message msg{};

        msg.msg_type  = static_cast<uint8_t>(*(p + Offset<T>::TYPE));
        msg.locate    = loadu_bswap<uint16_t>(p + Offset<T>::LOCATE);
        msg.symbol    = loadu_bswap<uint64_t>(p + Offset<T>::SYMBOL);

        return msg;
    }
};

template <>
struct Decoder<SystemEvent> {};

// Force inlining the decode functions could put pressure on I-cache, measure this later. Maybe only inline the hot decodes? (add/cancel/delete)
// maybe don't flatten process_msg() because of this... don't want the unlikely cases pressuring I-cache for no reason, jump to those when we hit those (rare)

// arg moldmsg* or std::byte* (meaning we pass address of ->data[])?
FORCE_INLINE Message deserialize(const moldmsg* __restrict__ mold_msg) noexcept
{
    const std::byte* __restrict__ p = reinterpret_cast<const std::byte*>(mold_msg->data);

    // measure with branch attributes for each case later
    
    switch (static_cast<char>(p[0])) {
        case 'A': return Decoder<Add>::decode(p);
        case 'X': return Decoder<Cancel>::decode(p);   
        case 'D': return Decoder<Delete>::decode(p);   
        case 'U': return Decoder<Replace>::decode(p);
        case 'E': 
        case 'C': return Decoder<Execute>::decode(p);

        // case 'S': [[unlikely]] Decoder<SystemEvent>::decode(p);
        default: /* parse_error() */ 
    }
}
