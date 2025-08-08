#pragma once

#include <concepts>
#include <vector>
#include <functional>
#include <span>

#include "socket_utils.hpp"
#include "socket_config.hpp"

namespace network::utilities 
{
    constexpr std::size_t kbuf_len{1<<12};
    constexpr std::size_t ubuf_len{1<<20};

    template <typename T>
    concept is_protocol = std::is_same_v<T, UDP> || std::is_same_v<T, TCP>;

    template <is_protocol T>
    struct Socket
    {
        int32_t socket_fd_{-1};
        std::function<void(Socket *, int64_t rx_time)> recv_callback_ = nullptr;
        std::vector<std::byte> send_buffer_;
        std::vector<std::byte> recv_buffer_;
        int32_t write_index_{0};
        int32_t read_index_{0};
        struct sockaddr_in socket_attributes_{};

        Socket() noexcept
        {
            send_buffer_.resize(ubuf_len);
            recv_buffer_.resize(ubuf_len);
        }

        Socket(const Socket&) = delete;
        Socket(const Socket&&) = delete;
        Socket &operator=(const Socket&) = delete;
        Socket &operator=(const Socket&&) = delete;

        auto connect(std::string_view ip, std::string_view interface, int32_t port, Listening listening, Blocking blocking) -> int32_t
        {
            const SocketConfig<T> socket_config{ip, interface, port, listening, blocking};
            socket_fd_ = create_and_set_socket(socket_config);
            
            socket_attributes_.sin_addr.s_addr = INADDR_ANY;
            socket_attributes_.sin_family = AF_INET;
            socket_attributes_.sin_port = htons(port);

            return socket_fd_;
        }


        auto send_and_recv() noexcept -> bool 
        {
            char ctrl_buf[CMSG_SPACE(sizeof(struct timeval))];
            auto cmsg = reinterpret_cast<struct cmsghdr *>(&ctrl_buf);

            iovec iov{recv_buffer_.data() + read_index_, ubuf_len - read_index_};
            msghdr msg{&socket_attributes_, sizeof(socket_attributes_), &iov, 1, &ctrl_buf, sizeof(ctrl_buf), 0};

            const auto read_len = recvmsg(socket_fd_, &msg, MSG_DONTWAIT);
            if (read_len > 0) {
                read_index_ += read_len;

                int64_t kernel_time = 0;
                timeval time_kernel;
                if (cmsg->cmsg_level == SOL_SOCKET &&
                    cmsg->cmsg_type == SCM_TIMESTAMP &&
                    cmsg->cmsg_len == CMSG_LEN(sizeof(time_kernel))) {
                        std::memcpy(&time_kernel, CMSG_DATA(cmsg), sizeof(time_kernel));
                        kernel_time = time_kernel.tv_sec * 1e9 + time_kernel.tv_usec * 1e3;
                    }
                
                recv_callback_(this, kernel_time);
            }

            if (write_index_ > 0)
                const auto n = ::send(socket_fd_, send_buffer_.data(), write_index_, MSG_DONTWAIT | MSG_NOSIGNAL);
            write_index_ = 0;

            return (read_len > 0);
        }

        auto send(std::span<const std::byte> buffer) noexcept -> int32_t 
        {
            if (write_index_ + buffer.size() > send_buffer_.size())
                return -1; // handle this error
            std::memcpy(send_buffer_.data() + write_index_, buffer.data(), buffer.size());
            write_index_ += buffer.size();
            return 0;
        }

    };
}
