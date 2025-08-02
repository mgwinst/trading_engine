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
        constexpr int32_t port_{-1};
        constexpr bool is_listening_{false};

        SocketConfig(std::string_view ip, std::string_view interface, int32_t port, bool is_listening) :
            ip_{ip}, interface_{interface}, port_{port}, is_listening_{is_listening} {}
            

        SocketConfig(std::string_view interface, int32_t port, bool is_listening) {
            ip_ = get_interface_ip(interface);
            macros::ASSERT(!ip_.empty(), std::format("Interface {} IP address not found", interface));
            interface_ = interface;
            port_ = port;
            is_listening_ = is_listening;
        }


        auto get_hints() const noexcept -> addrinfo
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

        auto to_string() const noexcept
        {
            return std::format("<Socket Configuration: (Interface=[{}], IP address=[{}], Port=[{}], Protocol=[{}], Listening=[{}])>",
                interface_, ip_, port_, std::is_same_v<T, TCP> ? "TCP" : "UDP", is_listening_);
        }
    };

    constexpr int32_t max_tcp_server_backlog = 1024;

    template <typename T>
    inline auto create_socket(const SocketConfig<T>& socket_config) -> int32_t
    {
        addrinfo hints = socket_config.get_hints();
        addrinfo* result_sa_list{nullptr};
        const auto status = getaddrinfo(socket_config.ip_.c_str(), std::to_string(socket_config.port_).c_str(), &hints, &result_sa_list);
        macros::ASSERT(status != -1, "getaddrinfo() failed" + gai_strerror(status));
        
        int32_t socket_fd{-1};
        int32_t flag{1};

        addrinfo* res = result_sa_list;
        for (addrinfo* res = result_sa_list; res != nullptr; res = res->ai_next) {
            if ((socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
                common::log_error("socket() failed");
                close(socket_fd);
                continue;
            }

            if (set_non_blocking(socket_fd) == -1) {
                common::log_error("set_non_blocking() failed");
                close(socket_fd);
                continue;
            }

            if (set_software_timestamp(socket_fd) == -1) {
                common::log_error("set_software_timestamp() failed");
                close(socket_fd);
                continue;
            }

            if constexpr (std::is_same_v<T, TCP>) {
                if (disable_nagle(socket_fd) == -1) {
                    common::log_error("disable_nagle() failed");
                    close(socket_fd);
                    continue;
                }
            }

            if (!socket_config.is_listening_) {
                if (connect(socket_fd, res->ai_addr, res->ai_addrlen) == -1)
                    common::log_error("connect() failed");
                    close(socket_fd);
                    continue;
            }

            if constexpr (socket_config.is_listening_) {
                if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1) {
                    common::log_error("setsockopt() failed");
                    close(socket_fd);
                    continue;
                }

                if (bind(socket_fd, res->ai_addr, res->ai_addrlen) == -1) {
                    common::log_error("bind() failed");
                    close(fd);
                    continue;
                }
            }

            if constexpr (std::is_same_v<T, TCP> || socket_config.is_listening_)
                if (listen(socket_fd, max_tcp_server_backlog) == -1) {
                    common::log_error("listen() failed");
                    close(fd);
                    continue;
                }
            }

            freeaddrinfo(result_sa_list);
            return socket_fd;
        }

        freeaddrinfo(result_sa_list);
        return -1;
    }







}