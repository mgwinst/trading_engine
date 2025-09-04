#pragma once

#include <cstdint>
#include <string_view>

namespace network
{
    struct RawSocket
    {
        RawSocket();
        RawSocket(std::string_view interface);

        ~RawSocket();

        RawSocket(const RawSocket&) = delete;
        RawSocket& operator=(const RawSocket&) = delete;
        RawSocket(RawSocket&&) = delete;
        RawSocket& operator=(RawSocket&&) = delete;

        auto readfrom() -> uint32_t;

        int32_t fd_;
        // spsc ring buffer
    };
} // end of namespace
