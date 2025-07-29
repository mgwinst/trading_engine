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
#include <string>
#include <cstring>
#include <format>
#include <print>

#include "common/macros.h"

namespace network {

    inline std::string get_iface_ip(const std::string_view interface) {
        char buffer[NI_MAXHOST] = {'\0'};
        ifaddrs* ifaddr = nullptr;
        
        if (getifaddrs(&ifaddr) != -1) {
            ifaddrs* ifa = ifaddr;
            while (ifa) {
                if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && interface == ifa->ifa_name) {
                    getnameinfo(ifa->ifa_addr, sizeof(sockaddr_in), buffer, sizeof(buffer), NULL, 0, NI_NUMERICHOST);
                    break;
                }
                ifa = ifa->ifa_next;
            }
        }

        freeifaddrs(ifaddr);
        return std::string{buffer};
    }

    struct SocketConfig {
        std::string ip{};
        std::string iface{};
        int32_t port{-1};
        bool is_udp{false};
        bool is_listening{false};

        auto to_string() const {
            return std::format("<Socket Configuration: (interface=[{}], ip address=[{}], port number=[{}], protocol=[{}], listening=[{}])>",
                iface, ip, port, (is_udp ? "UDP" : "TCP"), is_listening);
        }
    };

    class SocketConfigFactory {
        static std::optional<SocketConfig> from_ip(const std::string& ip, int32_t port, bool is_udp, bool is_listening) {
            if (port <= 0 || port > 65535) return std::nullopt;
            if (ip.empty()) return std::nullopt;
            return SocketConfig{ip, "", port, is_udp, is_listening};
        }

        static std::optional<SocketConfig> from_interface(const std::string& iface, int32_t port, bool is_udp, bool is_listening) {
            if (port <= 0 || port > 65535) return std::nullopt;

            const auto ip = get_iface_ip(iface);
            if (ip.empty()) return std::nullopt;

            return SocketConfig{ip, iface, port, is_udp, is_listening};
        }
    };

    class Socket {
    public: 
        Socket() noexcept = default;
        Socket(int32_t) noexcept;
        Socket(Socket&&) noexcept;
        Socket& operator=(Socket&&) noexcept;
        ~Socket();

        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;

        bool set_non_blocking();
        bool disable_nagle();
        bool set_so_time_stamp();
        bool would_block();
        bool set_unicast_ttl(int ttl);
        bool set_mcast_ttl(int mcast_ttl);
        bool join_mcast(const std::string& ip);
        auto get_fd() const { return _fd; }
        auto release_fd();
        
    private:
        int32_t _fd{-1};
    };

    inline addrinfo get_hints(const SocketConfig& socket_config) {
        addrinfo hints{}; // C++ standard ensures POD type will zero out members, don't need to call memset(&hints, 0, sizeof(hints))
        hints.ai_family = AF_UNSPEC; // handle both iPv4 and iPv6
        hints.ai_socktype = socket_config.is_udp ? SOCK_DGRAM : SOCK_STREAM;
        hints.ai_protocol = socket_config.is_udp ? IPPROTO_UDP : IPPROTO_TCP;
        hints.ai_flags = (socket_config.is_listening ? AI_PASSIVE : 0) | AI_NUMERICHOST | AI_NUMERICSERV;
        return hints;
    }

    inline Socket create_socket(const SocketConfig& socket_config) {
        const auto ip = socket_config.ip.empty() ? get_iface_ip(socket_config.iface) : socket_config.ip;
        addrinfo hints = get_hints(socket_config);
        addrinfo* sa_list{nullptr};
        
    }


}