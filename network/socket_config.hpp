#pragma once

#include <type_traits>
#include <system_error>
#include <expected>

#include "socket_utils.hpp"

#define MAX_TCP_SERVER_BACKLOG 1024

namespace network::utilities 
{
    struct TCP {}; 
    struct UDP {};

    enum class Listening {YES, NO};
    enum class Blocking {YES, NO};
    
    template <typename T>
    struct SocketConfig 
    {
        std::string ip_;
        std::string interface_;
        int32_t port_;
        Listening listening_;
        Blocking blocking_;

        SocketConfig(std::string_view ip, std::string_view interface, int32_t port, Listening listening, Blocking blocking) 
        {
            if (ip.empty()) {
                ip_ = get_interface_ip(interface);
                macros::ASSERT(!ip_.empty(), std::format("Interface: [{}] IP address not found", interface), macros::SOURCE_LOCATION());
                interface_ = interface;
            } else {
                ip_ = ip;
                interface_ = "";
            }

            port_ = port;
            listening_ = listening;
            blocking_ = blocking;
        }

        auto to_string() const noexcept
        {
            return std::format("<Socket Configuration: (Interface=[{}], IP address=[{}], Port=[{}], Protocol=[{}], Listening=[{}], Blocking=[{}])>",
                interface_, 
                ip_, 
                port_, 
                std::is_same_v<T, TCP> ? "TCP" : "UDP",
                listening_ == Listening::YES ? "true" : "false", 
                blocking_ == Blocking::YES ? "true" : "false"
            );
        }
    };

    template <typename T>
    auto get_hints(const SocketConfig<T>& socket_config) noexcept -> addrinfo
    {
        addrinfo hints{}; 
        hints.ai_family = AF_INET;
        hints.ai_flags = (socket_config.listening_ == Listening::YES ? AI_PASSIVE : 0) | AI_NUMERICHOST | AI_NUMERICSERV;

        if constexpr (std::is_same_v<T, TCP>) {
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
        } else {
            hints.ai_socktype = SOCK_DGRAM;
            hints.ai_protocol = IPPROTO_UDP;
        }

        return hints;
    }

    template <typename T>
    inline auto create_and_set_socket(const SocketConfig<T>& socket_config) -> int32_t
    {
        addrinfo hints = get_hints(socket_config);
        addrinfo* result_sa_list{nullptr};
        const auto status = getaddrinfo(socket_config.ip_.c_str(), std::to_string(socket_config.port_).c_str(), &hints, &result_sa_list);
        macros::ASSERT(status != -1, std::string{"getaddrinfo() failed"} + gai_strerror(status), macros::SOURCE_LOCATION());
        
        int32_t socket_fd{-1};
        int32_t flag{1};

        addrinfo* res = result_sa_list;
        for (addrinfo* res = result_sa_list; res != nullptr; res = res->ai_next) {
            if ((socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
                macros::LOG_ERROR("socket() failed", macros::SOURCE_LOCATION(), errno);
                close(socket_fd);
                continue;
            }

            if (socket_config.blocking_ == Blocking::NO) {
                if (set_non_blocking(socket_fd) == -1) {
                    macros::LOG_ERROR("set_non_blocking() failed", macros::SOURCE_LOCATION(), errno);
                    close(socket_fd);
                    continue;
                }
            }

            if (set_software_timestamp(socket_fd) == -1) {
                macros::LOG_ERROR("set_software_timestamp() failed", macros::SOURCE_LOCATION(), errno);
                close(socket_fd);
                continue;
            }

            if constexpr (std::is_same_v<T, TCP>) {
                if (disable_nagle(socket_fd) == -1) {
                    macros::LOG_ERROR("disable_nagle() failed", macros::SOURCE_LOCATION(), errno);
                    close(socket_fd);
                    continue;
                }
            }

            if (socket_config.listening_ == Listening::NO) {
                if (connect(socket_fd, res->ai_addr, res->ai_addrlen) == -1) {
                    if (errno != EINPROGRESS) {
                        macros::LOG_ERROR("connect() failed", macros::SOURCE_LOCATION(), errno);
                        close(socket_fd);
                        continue;
                    }

                    fd_set write_fds;
                    FD_ZERO(&write_fds);
                    FD_SET(socket_fd, &write_fds); // add socket_fd to set of fd's to monitor
                    struct timeval timeout {.tv_sec = 3, .tv_usec = 0};
                    if (select(socket_fd + 1, nullptr, &write_fds, nullptr, &timeout) == -1) {
                        macros::LOG_ERROR("select() failed", macros::SOURCE_LOCATION(), errno);
                        close(socket_fd);
                        continue;
                    }

                    if (FD_ISSET(socket_fd, &write_fds)) {
                        int error = 0;
                        socklen_t error_len = sizeof(error);
                        if (setsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, error_len) == -1) {
                            macros::LOG_ERROR("setsockopt() failed", macros::SOURCE_LOCATION(), errno);
                            close(socket_fd);
                            continue;
                        }
                    }
                }
            }

            if (socket_config.listening_ == Listening::YES) {
                if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1) {
                    macros::LOG_ERROR("setsockopt() failed", macros::SOURCE_LOCATION(), errno);
                    close(socket_fd);
                    continue;
                }

                if (bind(socket_fd, res->ai_addr, res->ai_addrlen) == -1) {
                    macros::LOG_ERROR("bind() failed", macros::SOURCE_LOCATION(), errno);
                    close(socket_fd);
                    continue;
                }
            }

            if (std::is_same_v<T, TCP> || socket_config.listening_ == Listening::YES) {
                if (listen(socket_fd, MAX_TCP_SERVER_BACKLOG) == -1) {
                    macros::LOG_ERROR("listen() failed", macros::SOURCE_LOCATION(), errno);
                    close(socket_fd);
                    continue;
                }
            }

            freeaddrinfo(result_sa_list);
            return socket_fd;
        }

        freeaddrinfo(result_sa_list);
        return socket_fd;
    }
}
