#pragma once

#include <string_view>
#include <cstdint>

#include "packet.hpp"

inline constexpr std::size_t QUEUE_SIZE{ 4 * 1024 * 1024 };

namespace network
{
    struct RawSocket
    {
        int32_t fd_{ -1 };
        Ring ring_;

        RawSocket(std::string_view interface);
        ~RawSocket();

        RawSocket(const RawSocket&) = delete;
        RawSocket& operator=(const RawSocket&) = delete;
        RawSocket(RawSocket&&) = delete;
        RawSocket& operator=(RawSocket&&) = delete;

        void read();
    };

} // namespace network