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

    class Socket {
    public: 
        Socket(const SocketConfig& socket_config) noexcept;
        Socket(Socket&& socket) noexcept;
        Socket& operator=(Socket&& socket) noexcept;
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
        
    private:
        int32_t _fd;
    };



}