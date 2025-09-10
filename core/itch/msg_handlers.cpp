#include <utility>
#include <memory>
#include <bit>
#include <type_traits>
#include <cstring>
#include <algorithm>

#include "msg_handlers.hpp"

namespace rng = std::ranges;

void system_event_handler(msgblk* msg)
{
    
}

void add_order_handler(msgblk* msg)
{
    static_assert(std::is_trivially_copyable_v<AddOrderMessage>, "AddOrderMessage must be trivially copyable");
    
    std::array<std::byte, sizeof(AddOrderMessage)> buffer;
    std::memcpy(buffer.data(), msg, sizeof(AddOrderMessage));
    auto add_order = std::bit_cast<AddOrderMessage>(buffer);
    
    add_order.stock_locate = std::byteswap(add_order.stock_locate);
    add_order.tracking_number = std::byteswap(add_order.tracking_number);
    add_order.reference_number = std::byteswap(add_order.reference_number);
    add_order.num_shares = std::byteswap(add_order.num_shares);
    add_order.price = std::byteswap(add_order.price);
    
    // pass this message to orderbook
}

void add_order_mpid_handler(msgblk* msg)
{

}

void cancel_order_handler(msgblk* msg)
{

}

void delete_order_handler(msgblk* msg)
{

}

void replace_order_handler(msgblk* msg)
{

}

const std::array<MessageHandler, num_handlers>& get_msg_handler_table()
{
    static constexpr std::array<MessageHandler, num_handlers> msg_handler_table = []() constexpr {
        std::array<MessageHandler, num_handlers> temp_table{};
        temp_table['S'] = &system_event_handler;
        temp_table['A'] = &add_order_handler;
        temp_table['F'] = &add_order_mpid_handler;
        temp_table['X'] = &cancel_order_handler;
        temp_table['D'] = &delete_order_handler;
        temp_table['U'] = &replace_order_handler;
        // ...

        return temp_table;
    }();

    return msg_handler_table;
}