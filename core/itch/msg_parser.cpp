#include <arpa/inet.h>

#include "common/macros.hpp"
#include "itch/msg_parser.hpp"
#include "itch/moldudp64.hpp"
#include "itch/msg_types.hpp"
#include "itch/msg_handlers.hpp"

namespace rng = std::ranges;

constexpr uint8_t session_num[10] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '1'};

// void simd_parse_mold_packet(std::byte* packet) 


void parse_mold_packet(std::byte* packet) 
{
    moldhdr* mold_hdr = reinterpret_cast<moldhdr *>(packet);
    
    if (std::memcmp(mold_hdr->session, session_num, 10))
        error_exit("session numbers don't match");

    auto msg_count = std::byteswap(mold_hdr->msg_count);
    if (msg_count == 0x0000) {
        // heartbeat
    } else if (msg_count == 0xFFFF) {
        // end of session
    }

    const auto msg_handlers = get_msg_handler_table();

    for (std::size_t i = 0; i < mold_hdr->msg_count; i++) {
        msgblk* msg = mold_hdr->msg_blk;
        msg_handlers[msg->data[0]](msg);
        msg = reinterpret_cast<msgblk *>(reinterpret_cast<uint8_t *>(msg) + msg->msg_len + sizeof(msg->msg_len));
    }
}

