#pragma once

#include <array>

#include "itch/moldudp64.hpp"

using MessageHandler = void(*)(msgblk*);

void system_event_handler(msgblk* msg);
void add_order_handler(msgblk* msg);
void add_order_mpid_handler(msgblk* msg);
void cancel_order_handler(msgblk* msg);
void delete_order_handler(msgblk* msg);
void replace_order_handler(msgblk* msg);
// ...

constexpr std::size_t num_handlers{ 128 };

const std::array<MessageHandler, num_handlers>& get_msg_handler_table();