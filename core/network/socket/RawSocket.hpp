#pragma once

#include <cstdint>
#include <string>

namespace network
{
    struct RawSocket
    {
        RawSocket();
        RawSocket(std::string_view interface);

        ~RawSocket();

        RawSocket(const RawSocket&) = delete;
        RawSocket& operator=(RawSocket&&) = delete;
        RawSocket(const RawSocket&) = delete;
        RawSocket& operator=(RawSocket&&) = delete;

        int32_t fd_;
    };

} // end of namespace
