#include <utility>
#include <variant>
#include <memory>
#include <cstring>
#include <algorithm>
#include <print>

#include "parser/msg_parser.hpp"
#include "parser/deserialize.hpp"

#include "common/queues/MessageQueues.hpp"

MessageVariant empty_handler(itchmsg* msg)
{
    return std::monostate{};
}

MessageVariant system_event_handler(itchmsg* msg)
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

    return system_event;
}

using StockLocateMap = std::unordered_map<int16_t, std::string>;

MessageVariant stock_directory_handler(itchmsg* msg)
{
    auto stock_directory_message = deserialize<StockDirectoryMessage>(msg);
    // stock_directory_message.stock_locate
    // stock_directory_message.symbol
    // populate the stock directory to symbol map
    return stock_directory_message;
}

MessageVariant add_order_handler(itchmsg* msg)
{
    return deserialize<AddOrderMessage>(msg);
}

MessageVariant cancel_order_handler(itchmsg* msg)
{   
    return deserialize<OrderCancelMessage>(msg);
}

MessageVariant delete_order_handler(itchmsg* msg)
{
    return deserialize<OrderDeleteMessage>(msg);
}

MessageVariant replace_order_handler(itchmsg* msg)
{
    return deserialize<OrderReplaceMessage>(msg);
}

const std::array<MessageHandler, num_handlers>& get_msg_handler_table()
{
    static std::array<MessageHandler, num_handlers> msg_handler_table = []() {
        std::array<MessageHandler, num_handlers> temp_table;
        
        for (auto& h : temp_table) {
            h = &empty_handler;
        }

        temp_table['S'] = &system_event_handler;
        temp_table['R'] = &stock_directory_handler;
        temp_table['A'] = &add_order_handler;
        temp_table['X'] = &cancel_order_handler;
        temp_table['D'] = &delete_order_handler;
        temp_table['U'] = &replace_order_handler;

        return temp_table;
    }();

    return msg_handler_table;
}

/*
class MessageParser
{
    
};
*/