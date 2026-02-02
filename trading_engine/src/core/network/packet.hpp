#pragma once

#include "../common/macros.hpp"

namespace network
{
    struct ipv4_min
    {
        uint8_t ver_ihl;
        uint8_t tos;
        uint16_t tot_len;
        uint16_t id;
        uint16_t frag_off;
        uint8_t ttl;
        uint8_t proto;
        uint16_t csum;
        uint32_t src_addr;
        uint32_t dst_addr;
    } PACKED;

    static_assert(sizeof(ipv4_min) == 20);

} // namespace network
