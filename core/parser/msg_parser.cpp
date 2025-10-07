#include <utility>
#include <variant>
#include <memory>
#include <cstring>
#include <algorithm>
#include <print>

#include "parser/msg_parser.hpp"
#include "parser/deserialize.hpp"

#include "common/queues/MessageQueues.hpp"

MessageVariant empty_handler(msgblk* msg)
{
    return std::monostate{};
}

MessageVariant system_event_handler(msgblk* msg)
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

MessageVariant stock_directory_handler(msgblk* msg)
{
    auto stock_directory_message = deserialize<StockDirectoryMessage>(msg);
    // stock_directory_message.stock_locate
    // stock_directory_message.symbol
    // populate the stock directory to symbol map
    return stock_directory_message;
}

MessageVariant add_order_handler(msgblk* msg)
{
    return deserialize<AddOrderMessage>(msg);
}

MessageVariant cancel_order_handler(msgblk* msg)
{   
    return deserialize<OrderCancelMessage>(msg);
}

MessageVariant delete_order_handler(msgblk* msg)
{
    return deserialize<OrderDeleteMessage>(msg);
}

MessageVariant replace_order_handler(msgblk* msg)
{
    return deserialize<OrderReplaceMessage>(msg);
}

const std::array<MessageHandler, num_handlers>& get_msg_handler_table()
{
    static constexpr std::array<MessageHandler, num_handlers> msg_handler_table = []() constexpr {
        std::array<MessageHandler, num_handlers> temp_table;
        
        temp_table.fill(&empty_handler);

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

using StockLocateMap = std::unordered_map<int16_t, std::string>;

std::array<uint8_t, 10>& get_session()
{
    static std::array<uint8_t, 10> session;
    return session;
}

void parse_mold_packet(moldhdr* mold_hdr)
{
    auto msg_count = std::byteswap(mold_hdr->msg_count);
    if (msg_count == 0x0000) [[unlikely]] {
        // heartbeat
    } else if (msg_count == 0xFFFF) [[unlikely]] {
        // end of session
    }

    const auto msg_handlers = get_msg_handler_table();

    for (std::size_t i = 0; i < mold_hdr->msg_count; i++) {
        msgblk* msg = mold_hdr->msg_blk;
        
        char msg_type = msg->data[0]; // first field in message byte stream

        auto msg_variant = msg_handlers[msg_type](msg);

        // std::string_view ticker = (extract from msg_variant)
        // queues[ticker]->try_push(std::move(msg_variant));

        msg = reinterpret_cast<msgblk *>(reinterpret_cast<uint8_t *>(msg) + msg->msg_len + sizeof(msg->msg_len));
    }
}

/*
class MessageParser
{
    
};
*/