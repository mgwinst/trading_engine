#pragma once

#include <cstdint>
#include <string_view>

#include "network/socket/tpacket.hpp"
#include "network/socket/socket_utils.hpp"

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

        void read();

        int32_t fd_{ -1 };
        Ring ring_{};
        
        // CircularBuffer<uint8_t, DEFAULT_BUFFER_SIZE> buffer_{};
        // tpacket_stats_v3 stats;
    };

} // namespace network
