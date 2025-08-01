#pragma once

#include <concepts>

#include "socket_utils.hpp"
#include "socket_config.hpp"

namespace network::utilities 
{
    struct TCP {};
    struct UDP {};

    template <typename T>
    concept is_protocol = std::is_same_v<T, UDP> || std::is_same_v<T, TCP>;

    template <is_protocol T>
    class Socket
    {
    public:
        Socket() noexcept;

        Socket(const Socket&) = delete;
        Socket(const Socket&&) = delete;
        Socket &operator=(const Socket&) = delete;
        Socket &operator=(const Socket&&) = delete;

        void connect(std::string_view ip, std::string_view interface, int32_t port, bool is_listening) 
        {
            const SocketConfig<T> socket_config{ip, interface, port, is_listening};
            socket_fd_ = create_socket(socket_config);
            
            
        }

        void connect(std::string_view interface, int32_t port, bool is_listening) 
        {
            const SocketConfig<T> socket_config{interface, port, is_listening};
        }

    private:
        int32_t socket_fd_{-1};
        std::vector<std::byte> send_buffer_;
        std::vector<std::byte> recv_buffer_;
        int32_t write_index_{0};
        int32_t read_index_{0};

        int32_t kernel_buffer_len_{1<<12};
        int32_t user_buffer_len_{1<<20};
    };
}
