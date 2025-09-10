#pragma once

#include <cstdint>

struct msgblk
{
    uint16_t msg_len;
    uint8_t* data{ nullptr };
};

struct moldhdr
{
    uint8_t session[10];
    uint64_t seq_num;
    uint16_t msg_count;
    msgblk* msg_blk;
};

struct request_packet
{
    uint8_t session[10];
    uint64_t seq_num;
    uint16_t req_msg_count;
};

