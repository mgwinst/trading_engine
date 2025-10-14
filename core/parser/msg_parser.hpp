#pragma once

#include <array>
#include <type_traits>
#include <cstring>
#include <bit>

#include "parser/BinaryFILE.hpp"
#include "parser/msg_types.hpp"

using MessageHandler = MessageVariant(*)(itchmsg*);

MessageVariant system_event_handler(itchmsg* msg);
MessageVariant stock_directory_handler(itchmsg* msg);
MessageVariant add_order_handler(itchmsg* msg);
MessageVariant cancel_order_handler(itchmsg* msg);
MessageVariant delete_order_handler(itchmsg* msg);
MessageVariant replace_order_handler(itchmsg* msg);

constexpr std::size_t num_handlers{ 256 };
const std::array<MessageHandler, num_handlers>& get_msg_handler_table();