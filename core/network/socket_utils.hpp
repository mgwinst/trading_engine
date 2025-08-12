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

    inline auto configure_tpacket_v3(int32_t fd) -> int32_t
    {
        int version = TPACKET_V3;
        if (setsockopt(fd, SOL_PACKET, PACKET_VERSION, &version, sizeof(version)) == -1)
            return -1;
        return 0;
    }

    inline auto create_tpacket_req(int32_t block_size, int32_t num_blocks, int32_t timeout) -> tpacket_req3
    {
        tpacket_req3 req{};
        req.tp_block_size = block_size;
        req.tp_block_nr = num_blocks;
        req.tp_frame_size = 2048; // ignored in v3
        req.tp_frame_nr = (req.tp_block_size * req.tp_block_nr) / req.tp_frame_size; // ignored in v3
        req.tp_retire_blk_tov = timeout;    // partially filled block is returned to user space (retirement timeout (60ms))
        req.tp_sizeof_priv = 0;        // no private data
        req.tp_feature_req_word = 0;   // no special features

        macros::ASSERT(req.tp_block_size % getpagesize() == 0, 
            "Block size must be multipleof page size", macros::SOURCE_LOCATION());

        return req;
    }

    inline auto configure_tpacket_v3_rx_buf(int32_t fd, const tpacket_req3& req) -> int32_t
    {
        if (setsockopt(fd, SOL_PACKET, PACKET_RX_RING, &req, sizeof(req)) == -1)
            return -1;
        return 0;
    }

    inline auto configure_tpacket_v3_tx_buf(int32_t fd, const tpacket_req3& req) -> int32_t
    {
        if (setsockopt(fd, SOL_PACKET, PACKET_TX_RING, &req, sizeof(req)) == -1)
            return -1;
        return 0;
    }

    // usually passing AF_PACKET == family, ETH_P_IP == protocol
    inline auto get_sockaddr_ll(std::string_view interface, uint16_t family, uint16_t protocol) -> sockaddr_ll
    {
        sockaddr_ll addr{};
        addr.sll_family = family;
        addr.sll_protocol = htons(protocol);
        addr.sll_ifindex = if_nametoindex(interface.data());
        macros::ASSERT(addr.sll_ifindex != 0, "if_nametoindex() failed", macros::SOURCE_LOCATION());

        return addr;
    }


}
