#pragma once

#include <type_traits>
#include <system_error>
#include <expected>

#include "socket_utils.hpp"

namespace network::utilities 
{
    template <typename T>
    struct SocketConfig 
    {
        std::string ip_{};
        std::string interface_{};
        int32_t port_{-1};
        bool is_listening_{false};

        SocketConfig(std::string_view ip, int32_t port, bool is_listening) noexcept 
        {
            ip_ = ip;
            port_ = port;
            is_listening_ = is_listening;
        }

        SocketConfig(std::string_view interface, int32_t port, bool is_listening) noexcept 
        {
            const auto ip = get_interface_ip(interface);
            macros::ASSERT(!ip.empty(), std::format("{} error: Interface '{}' IP address not found", macros::SOURCE_LOCATION, interface));

            ip_ = ip;
            interface_ = interface;
            port_ = port;
            is_listening_ = is_listening;
        }

        addrinfo get_hints() const noexcept
        {
            addrinfo hints{}; 
            hints.ai_family = AF_INET;
            hints.ai_flags = (is_listening_ ? AI_PASSIVE : 0) | AI_NUMERICHOST | AI_NUMERICSERV;

            if constexpr (std::is_same_v<T, TCP>) {
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_protocol = IPPROTO_TCP;
            } else constexpr {
                hints.ai_socktype = SOCK_DGRAM;
                hints.ai_protocol = IPPROTO_UDP;
            }

            return hints;
        }

        auto to_string() const 
        {
            return std::format("<Socket Configuration: (Interface=[{}], IP address=[{}], Port=[{}], Protocol=[{}], Listening=[{}])>",
                interface_, ip_, port_, std::is_same_v<T, TCP> ? "TCP" : "UDP", is_listening_);
        }
    };





    constexpr int32_t max_tcp_server_backlog = 1024;

    template <typename T>
    inline int32_t create_socket(const SocketConfig<T>& socket_config)
    {
        addrinfo hints = socket_config.get_hints();
        addrinfo* result_sa_list{nullptr};
        const auto status = getaddrinfo(socket_config.ip_.c_str(), std::to_string(socket_config.port_).c_str(), &hints, &result_sa_list);
        macros::ASSERT(status != -1, std::format("{} [getaddrinfo() failed] {}: error: {} [{}] ", macros::SOURCE_LOCATION(), gai_strerror(status), std::strerror(errno), errno));
        
        int32_t socket_fd{-1};
        int32_t flag{1};

        addrinfo* res = result_sa_list;
        while (res) {
            socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)
            if (socket_fd == -1)

            set_non_blocking(socket_fd);
            set_software_timestamp(socket_fd);

            // if UDP
            disable_nagle(socket_fd);

            if (!socket_config.is_listening_) {
                connect(socket_fd, res->ai_addr, res->ai_addrlen);
            }

            if (socket_config.is_listening_) {
                setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
                bind(socket_fd, res->ai_addr, res->ai_addrlen);
            }

            // if TCP and listening
            listen(socket_fd, max_tcp_server_backlog);
        }

        return socket_fd;
    }
}