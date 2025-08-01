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
#include <iostream>
#include <string>
#include <cstring>
#include <expected>
#include <system_error>

#include "common/macros.hpp"
#include "utils.hpp"

namespace network::utilities 
{
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

    inline int32_t set_so_time_stamp(int32_t fd) noexcept
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

    // ***************************************************************************************************************

    enum class TransportLayerProtocol { TCP, UDP };

    struct SocketConfig 
    {
        std::string ip_{};
        std::string iface_{};
        int32_t port_{-1};
        TransportLayerProtocol protocol_;
        bool is_listening_{false};

        auto to_string() const {
            return std::format("<Socket Configuration: (Interface=[{}], IP address=[{}], Port=[{}], Protocol=[{}], Listening=[{}])>",
                iface_, ip_, port_, (protocol_ == TransportLayerProtocol::TCP ? "TCP" : "UDP"), is_listening_);
        }
    };

    enum class SocketConfigError { Invalid_Port, Interface_IP_Not_Found };

    struct SocketConfigFactory 
    {
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

    inline addrinfo get_hints(const SocketConfig& socket_config) 
    {
        addrinfo hints{}; 
        hints.ai_family = AF_UNSPEC; // handle both iPv4/iPv6
        hints.ai_socktype = socket_config.protocol_ == TransportLayerProtocol::TCP ? SOCK_STREAM : SOCK_DGRAM;
        hints.ai_protocol = socket_config.protocol_ == TransportLayerProtocol::TCP ? IPPROTO_TCP : IPPROTO_UDP;
        hints.ai_flags = (socket_config.is_listening_ ? AI_PASSIVE : 0) | AI_NUMERICHOST | AI_NUMERICSERV;
        return hints;
    }

    class Socket 
    {
    public:
        Socket() noexcept;
        Socket(int32_t) noexcept;
        Socket(Socket&&) noexcept;
        Socket& operator=(Socket&&) noexcept;
        ~Socket() noexcept;

        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;

        auto get_fd() const noexcept;
        auto release_fd() noexcept;

    private:
        int32_t fd_{-1};       
    };

    constexpr int32_t max_tcp_server_backlog = 1024;

    inline Socket create_socket(const SocketConfig& socket_config)
    {
        addrinfo hints = get_hints(socket_config);
        addrinfo* result_sa_list{nullptr};
        const auto status = getaddrinfo(socket_config.ip_.c_str(), std::to_string(socket_config.port_).c_str(), &hints, &result_sa_list);
        macros::ASSERT(status != -1, std::format("{} [getaddrinfo() failed] {}: error: {} [{}] ", macros::SOURCE_LOCATION(), gai_strerror(status), std::strerror(errno), errno));
        
        int32_t socket_fd{-1};
        int32_t flag{1};

        addrinfo* res = result_sa_list;
        while (res) {
            macros::ASSERT((socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) != -1,
                std::format("{} [socket() failed] error: {} [{}] ", macros::SOURCE_LOCATION(), std::strerror(errno), errno));

            macros::ASSERT(set_non_blocking(socket_fd) != -1,
                std::format("{} [set_non_blocking() failed] error: {} [{}] ", macros::SOURCE_LOCATION(), std::strerror(errno), errno));
            
            if (socket_config.protocol_ == TransportLayerProtocol::UDP) {
                macros::ASSERT(disable_nagle(socket_fd) != -1, 
                    std::format("{} [disable_nagle() failed] error: {} [{}] ", macros::SOURCE_LOCATION(), std::strerror(errno), errno));
            }

            if (!socket_config.is_listening_) {
                macros::ASSERT(connect(socket_fd, res->ai_addr, res->ai_addrlen) != -1,
                    std::format("{} [connect() failed] error: {} [{}] ", macros::SOURCE_LOCATION(), std::strerror(errno), errno));
            }

            if (socket_config.is_listening_) {
                macros::ASSERT(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) != -1,
                    std::format("{} [setsockopt() failed] error: {} [{}] ", macros::SOURCE_LOCATION(), std::strerror(errno), errno));

                macros::ASSERT(bind(socket_fd, res->ai_addr, res->ai_addrlen) != -1,
                    std::format("{} [bind() failed] error: {} [{}] ", macros::SOURCE_LOCATION(), std::strerror(errno), errno));
            }

            if (socket_config.protocol_ == TransportLayerProtocol::TCP && socket_config.is_listening_) {
                macros::ASSERT(listen(socket_fd, max_tcp_server_backlog) != -1, 
                    std::format("{} [bind() failed] error: {} [{}] ", macros::SOURCE_LOCATION(), std::strerror(errno), errno));
            }

            macros::ASSERT(set_so_time_stamp(socket_fd) != -1,
                std::format("{} [set_so_time_stamp() failed] error: {} [{}] ", macros::SOURCE_LOCATION(), std::strerror(errno), errno));
        }
        
        return Socket{socket_fd};
    }


}