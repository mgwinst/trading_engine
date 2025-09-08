#pragma once

#include <cstdint>

struct moldhdr
{
    char session[10];
    uint64_t seq_num;
    uint16_t msg_count;
    
    struct msgblk
    {
        uint16_t msg_len;
        void* data{ nullptr };
    } msg_blk;
};

struct request_packet
{
    char session[10];
    uint64_t seq_num;
    uint16_t req_msg_count;
};

