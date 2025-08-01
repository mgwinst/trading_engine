#pragma once

#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <format>
#include <string>
#include <cstring>

#include "common/macros.hpp"

namespace network::utilities
{
    inline std::string get_interface_ip(const std::string_view interface) 
    {
        char buffer[NI_MAXHOST] = {'\0'};
        ifaddrs* ifaddr = nullptr;
        
        if (getifaddrs(&ifaddr) == -1) {
            return "";
        }
        
        ifaddrs* ifa = ifaddr;
        while (ifa) {
            if (ifa->ifa_addr && interface == ifa->ifa_name && ifa->ifa_addr->sa_family == AF_INET) {
                if (getnameinfo(ifa->ifa_addr, sizeof(sockaddr_in), buffer, sizeof(buffer), nullptr, 0, NI_NUMERICHOST) == 0) {
                    break;
                }
            }
            ifa = ifa->ifa_next;
        }

        freeifaddrs(ifaddr);
        return std::string{buffer};
    }

    inline int32_t set_non_blocking(int32_t fd) noexcept
    {
        const auto flags = fcntl(fd, F_GETFL, 0); // this returns the flags associated with fd currently
        if (flags & O_NONBLOCK) 
            return 0;
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
            return -1;
        return 0;
    }

    // implicit reinterpret_cast<void*> when passing to const void* optval in C++
    inline int32_t disable_nagle(int32_t fd) noexcept
    {
        int32_t flag = 1;
        if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag)) == -1)
            return -1;
        return 0;
    }

    inline int32_t set_software_timestamp(int32_t fd) noexcept
    {
        int32_t flag = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP, &flag, sizeof(flag)) == -1)
            return -1;
        return 0;
    }

    inline bool would_block() noexcept
    {
        return (errno == EWOULDBLOCK || errno == EINPROGRESS);
    }

    inline int32_t set_unicast_ttl(int32_t fd, int32_t ttl) noexcept
    {
        if (setsockopt(fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1)
            return -1;
        return 0;
    }

    inline int32_t set_mcast_ttl(int32_t fd, int32_t ttl) noexcept
    {
        if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) == -1)
            return -1;
        return 0;
    }

    inline int32_t join_mcast(int32_t fd, const std::string &ip) noexcept
    {
        const ip_mreq mreq{{inet_addr(ip.c_str())}, {htonl(INADDR_ANY)}};
        if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) == -1)
            return -1;
        return 0;
    }
}