#include <arpa/inet.h>
#include <cstring>

#include "common/macros.hpp"
#include "common/CircularBuffer.hpp"
#include "itch/msg_parser.hpp"
#include "itch/moldudp64.hpp"
#include "itch/msg_types.hpp"
#include "itch/msg_handler.hpp"

uint8_t session_num[10] = {'0'};

void parse_mold_packet(moldhdr* mold_hdr)
{
    if (std::memcmp(mold_hdr->session, session_num, 10)) [[unlikely]]
        error_exit("session numbers don't match");

    auto msg_count = std::byteswap(mold_hdr->msg_count);
    if (msg_count == 0x0000) {
        // heartbeat
    } else if (msg_count == 0xFFFF) [[unlikely]] {
        // end of session
    }

    const auto msg_handlers = get_msg_handler_table();

    for (std::size_t i = 0; i < mold_hdr->msg_count; i++) {
        msgblk* msg = mold_hdr->msg_blk;
        msg_handlers[msg->data[0]](msg); // we want this to be non-blocking
        msg = reinterpret_cast<msgblk *>(reinterpret_cast<uint8_t *>(msg) + msg->msg_len + sizeof(msg->msg_len));
    }
}

