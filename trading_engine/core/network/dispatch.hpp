#pragma once

#include <array>

#include "protocol/deserialize.hpp"
#include "protocol/message.hpp"
#include "../common/queues/MessageQueuePool.hpp"
#include "../orderbook/symbol_directory.hpp"

template <SupportedMessageType T>
inline void Handler(itchmsg* itch_msg, MessageQueuePool<Message>& queues)
{

}

template <>
inline void Handler<SystemEventMessage>(itchmsg* itch_msg, MessageQueuePool<Message>& queues)
{
    auto msg = deserialize<SystemEventMessage>(itch_msg);

    switch (msg.event_code) {
        case ('O'):
            // start of day
            break;
        case ('S'):
            // start of system hours
            break;
        case ('Q'):
            // start of market hours
            break;
        case ('M'):
            // end of market hours
            break;
        case ('E'):
            // end of system hours
            break;
        case ('C'):
            // end of day
            break;
        default:
            break;
    }
}

template <>
inline void Handler<StockDirectoryMessage>(itchmsg* itch_msg, MessageQueuePool<Message>& queues)
{
    auto msg = deserialize<StockDirectoryMessage>(itch_msg);

}

template <>
inline void Handler<AddOrderMessage>(itchmsg* itch_msg, MessageQueuePool<Message>& queues)
{
    auto msg = deserialize<AddOrderMessage>(itch_msg);

}

template <>
inline void Handler<OrderCancelMessage>(itchmsg* itch_msg, MessageQueuePool<Message>& queues)
{
    auto msg = deserialize<OrderCancelMessage>(itch_msg);

}

template <>
inline void Handler<OrderDeleteMessage>(itchmsg* itch_msg, MessageQueuePool<Message>& queues)
{
    auto msg = deserialize<OrderDeleteMessage>(itch_msg);

}

template <>
inline void Handler<OrderReplaceMessage>(itchmsg* itch_msg, MessageQueuePool<Message>& queues)
{
    auto msg = deserialize<OrderReplaceMessage>(itch_msg);

}

using Func = void (*)(itchmsg*, MessageQueuePool<Message>&);

inline constexpr std::array<Func, 256> MessageHandlers = [] {
    std::array<Func, 256> table{};

    table['S'] = &Handler<SystemEventMessage>;
    table['R'] = &Handler<StockDirectoryMessage>;
    table['A'] = &Handler<AddOrderMessage>;
    table['X'] = &Handler<OrderCancelMessage>;
    table['D'] = &Handler<OrderDeleteMessage>;
    table['U'] = &Handler<OrderReplaceMessage>;

    return table;
}();

inline void dispatch_msg(char msg_type, itchmsg* itch_msg, MessageQueuePool<Message>& queues)
{
    MessageHandlers[msg_type](itch_msg, queues);
}
