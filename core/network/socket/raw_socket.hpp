#pragma once

#include <cstdint>
#include <string_view>

#include "tpacket.hpp"
#include "socket_utils.hpp"

namespace network
{
    struct RawSocket
    {
        RawSocket(std::string_view interface);
        ~RawSocket();

        RawSocket(const RawSocket&) = delete;
        RawSocket& operator=(const RawSocket&) = delete;
        RawSocket(RawSocket&&) = delete;
        RawSocket& operator=(RawSocket&&) = delete;

        int32_t fd_;
        Ring ring_{};
        // tpacket_stats_v3 stats;
    };

} // namespace network
