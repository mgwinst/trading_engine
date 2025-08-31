#pragma once

#include <cstdint>

struct mold_udp_64_hdr
{
    char session[10];
    uint64_t seq_num;
    uint16_t msg_count;
    
    struct message_block
    {
        uint16_t msg_len;
        void* data{ nullptr };
    } msg_block;
};

struct request_packet
{
    char session[10];
    uint64_t seq_num;
    uint16_t req_msg_count;
};

