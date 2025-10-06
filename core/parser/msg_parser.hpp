#pragma once

#include <array>
#include <type_traits>
#include <cstring>
#include <bit>

#include "parser/moldudp64.hpp"
#include "parser/msg_types.hpp"

void parse_mold_packet(moldhdr* mold_hdr);

using MessageHandler = MessageVariant(*)(msgblk*);

MessageVariant system_event_handler(msgblk* msg);
MessageVariant stock_directory_handler(msgblk* msg);
MessageVariant add_order_handler(msgblk* msg);
MessageVariant cancel_order_handler(msgblk* msg);
MessageVariant delete_order_handler(msgblk* msg);
MessageVariant replace_order_handler(msgblk* msg);

constexpr std::size_t num_handlers{ 256 };
const std::array<MessageHandler, num_handlers>& get_msg_handler_table();

constexpr std::size_t get_variant_index(char key);