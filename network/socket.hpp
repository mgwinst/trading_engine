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

// ***************************************************************************************************************

namespace network::utilities 
{
    inline void set_non_blocking(int32_t fd) noexcept
    {
        const auto flags = fcntl(fd, F_GETFL, 0); // this returns the flags associated with fd currently
        if (flags & O_NONBLOCK) return;
        macros::ASSERT(fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1, 
            std::format("{} [fcntl() failed] error: {} [{}] ", macros::SOURCE_LOCATION(), std::strerror(errno), errno));
    }

    // implicit reinterpret_cast<void*> when passing to const void* optval in C++
    inline void disable_nagle(int32_t fd) noexcept
    {
        int32_t flag = 1;
        macros::ASSERT(setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag)) == -1,
            std::format("{} [setsockopt() failed] error: {} [{}] ", macros::SOURCE_LOCATION(), std::strerror(errno), errno));
    }

    inline void set_so_time_stamp(int32_t fd) noexcept
    {
        int32_t flag = 1;
        macros::ASSERT(setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP, &flag, sizeof(flag)) != -1,
            std::format("{} [setsockopt() failed] error: {} [{}] ", macros::SOURCE_LOCATION(), std::strerror(errno), errno));
    }

    inline bool would_block() noexcept
    {
        return (errno == EWOULDBLOCK || errno == EINPROGRESS);
    }

    inline void set_unicast_ttl(int32_t fd, int32_t ttl) noexcept
    {
        macros::ASSERT(setsockopt(fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) != -1,
            std::format("{} [setsockopt() failed] error: {} [{}] ", macros::SOURCE_LOCATION(), std::strerror(errno), errno));
    }

    inline void set_mcast_ttl(int32_t fd, int32_t ttl) noexcept
    {
        macros::ASSERT(setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) != -1, 
            std::format("{} [setsockopt() failed] error: {} [{}] ", macros::SOURCE_LOCATION(), std::strerror(errno), errno));
    }

    inline void join_mcast(int32_t fd, const std::string &ip) noexcept
    {
        const ip_mreq mreq{{inet_addr(ip.c_str())}, {htonl(INADDR_ANY)}};
        macros::ASSERT(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) != -1,
            std::format("{} [setsockopt() failed] error: {} [{}] ", macros::SOURCE_LOCATION(), std::strerror(errno), errno));
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
        Socket() noexcept = default;
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

    /*
    inline Socket create_socket(const SocketConfig& socket_config)
    {
        addrinfo hints = get_hints(socket_config);
        addrinfo* result_sa_list{nullptr};
        const auto status = getaddrinfo(socket_config.ip_.c_str(), std::to_string(socket_config.port_).c_str(), &hints, &result_sa_list);
        common::ASSERT(status != 0, "") 
        
        int32_t socket_fd{-1};
        int32_t flag{1};

        addrinfo* res = result_sa_list;
        while (res) {
            socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
            if (socket_fd) {
                
            }
            
            
                
            
            

            
        }
        
        
        return Socket{socket_fd};
    }
    */


}