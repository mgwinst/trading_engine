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
#include <string_view>

#include "common/macros.hpp"
#include <linux/if_packet.h>
#include <linux/if_ether.h>

namespace network::utilities
{
    inline auto get_interface_ip(std::string_view interface) noexcept -> std::string
    {
        char buffer[NI_MAXHOST] = {'\0'};
        ifaddrs* ifaddr = nullptr;
        
        if (getifaddrs(&ifaddr) == -1) {
            return std::string{};
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

    inline auto set_non_blocking(int32_t fd) noexcept -> int32_t
    {
        const auto flags = fcntl(fd, F_GETFL, 0); // this returns the flags associated with fd currently
        if (flags & O_NONBLOCK) 
            return 0;
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
            return -1;
        return 0;
    }

    // implicit reinterpret_cast<void*> when passing to const void* optval in C++
    inline auto disable_nagle(int32_t fd) noexcept -> int32_t
    {
        int32_t flag = 1;
        if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag)) == -1)
            return -1;
        return 0;
    }

    inline auto set_software_timestamp(int32_t fd) noexcept -> int32_t
    {
        int32_t flag = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP, &flag, sizeof(flag)) == -1)
            return -1;
        return 0;
    }

    // inline auto set_hardware_timestamp(int32_t fd) noexcept -> int32_t

    inline auto would_block() noexcept -> bool
    {
        return (errno == EWOULDBLOCK || errno == EINPROGRESS);
    }

    inline auto set_unicast_ttl(int32_t fd, int32_t ttl) noexcept -> int32_t
    {
        if (setsockopt(fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1)
            return -1;
        return 0;
    }

    inline auto set_mcast_ttl(int32_t fd, int32_t ttl) noexcept -> int32_t
    {
        if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) == -1)
            return -1;
        return 0;
    }

    inline auto join_mcast(int32_t fd, const std::string &ip) noexcept -> int32_t
    {
        const ip_mreq mreq{{inet_addr(ip.c_str())}, {htonl(INADDR_ANY)}};
        if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) == -1)
            return -1;
        return 0;
    }

}
