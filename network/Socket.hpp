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
#include <expected>

#include "common/macros.hpp"

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

    enum struct TransportLayerProtocol { TCP, UDP };

    struct SocketConfig {
        std::string ip{};
        std::string iface{};
        int32_t port{-1};
        TransportLayerProtocol protocol;
        bool is_listening{false};

        auto to_string() const {
            return std::format("<Socket Configuration: (Interface=[{}], IP address=[{}], Port=[{}], Protocol=[{}], Listening=[{}])>",
                iface, ip, port, (protocol == TransportLayerProtocol::TCP ? "TCP" : "UDP"), is_listening);
        }
    };

    enum struct SocketConfigError { Invalid_Port, Interface_IP_Not_Found };

    struct SocketConfigFactory {
        static std::expected<SocketConfig, SocketConfigError> from_ip(const std::string& ip, int32_t port, TransportLayerProtocol proto, bool is_listening) {
            if (port < 1024 || port > 65535)
                return std::unexpected{SocketConfigError::Invalid_Port};

            return SocketConfig{ip, "", port, proto, is_listening};
        }

        static std::expected<SocketConfig, SocketConfigError> from_interface(const std::string& iface, int32_t port, TransportLayerProtocol proto, bool is_listening) {
            if (port < 1024 || port > 65535)
                return std::unexpected{SocketConfigError::Invalid_Port};

            const auto ip = get_iface_ip(iface);
            if (ip.empty())
                return std::unexpected{SocketConfigError::Interface_IP_Not_Found};

            return SocketConfig{ip, iface, port, proto, is_listening};
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
        hints.ai_socktype = socket_config.protocol == TransportLayerProtocol::TCP ? SOCK_STREAM : SOCK_DGRAM;
        hints.ai_protocol = socket_config.protocol == TransportLayerProtocol::TCP ? IPPROTO_TCP : IPPROTO_UDP;
        hints.ai_flags = (socket_config.is_listening ? AI_PASSIVE : 0) | AI_NUMERICHOST | AI_NUMERICSERV;
        return hints;
    }

    inline std::optional<Socket> create_socket(const SocketConfig& socket_config) {
        addrinfo hints = get_hints(socket_config);
        addrinfo* sa_list{nullptr};
        const auto ret = getaddrinfo(socket_config.ip.c_str(), std::to_string(socket_config.port).c_str(), &hints, &sa_list);
        common::ASSERT(ret == 0, std::format("getaddrinfo() failed. error: {}, errno: {}", gai_strerror(ret), strerror(errno)));
        if (ret == 0) return std::nullopt;

        return Socket{};
    }


}