#include <utility>
#include <memory>
#include <bit>
#include <type_traits>
#include <cstring>
#include <algorithm>
#include <print>

#include "itch/msg_handlers.hpp"
#include "itch/msg_types.hpp"
#include "itch/msg_parser.hpp"

namespace rng = std::ranges;

void empty_handler(msgblk* msg)
{

}

void system_event_handler(msgblk* msg)
{
    auto system_event = deserialize<SystemEventMessage>(msg);

    switch (system_event.event_code) {
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

void add_order_handler(msgblk* msg)
{
    auto add_order = deserialize<AddOrderMessage>(msg);
}

void cancel_order_handler(msgblk* msg)
{   
    auto order_cancel = deserialize<OrderCancelMessage>(msg);
}

void delete_order_handler(msgblk* msg)
{
    auto order_delete = deserialize<OrderDeleteMessage>(msg);
}

void replace_order_handler(msgblk* msg)
{
    auto order_replace = deserialize<OrderReplaceMessage>(msg);
}

const std::array<MessageHandler, num_handlers>& get_msg_handler_table()
{
    static constexpr std::array<MessageHandler, num_handlers> msg_handler_table = []() constexpr {
        std::array<MessageHandler, num_handlers> temp_table{};

        for (size_t i = 0; i < num_handlers; i++)
            temp_table[i] = &empty_handler;

        temp_table['S'] = &system_event_handler;
        temp_table['A'] = &add_order_handler;
        temp_table['X'] = &cancel_order_handler;
        temp_table['D'] = &delete_order_handler;
        temp_table['U'] = &replace_order_handler;

        return temp_table;
    }();

    return msg_handler_table;
}
