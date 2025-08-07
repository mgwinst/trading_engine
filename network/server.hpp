#pragma once

#include <socket.hpp>

namespace network::utilities 
{
    template <typename T>
    struct Server {
        int32_t epoll_fd_{-1};
        Socket<T> listener_socket_;
        epoll_event events_[1024];
        std::vector<Socket<T> *> recv_sockets_;
        std::vector<Socket<T> *> send_sockets_;
        std::function<void(Socket<T> *, int64_t rx_time)> recv_callback_ = nullptr;
        std::function<void> recv_finished_callback_ = nullptr;

        auto add_to_epoll_list(Socket<T>* socket) -> int32_t
        {
            epoll_event ev{EPOLLET | EPOLLIN, socket}; // play around with edge vs level triggered
            if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket->socket_fd_, &ev) == -1)
                return -1;
            return 0;
        }

        auto listen(std::string_view interface, int32_t port) -> void
        {
            epoll_fd_ = epoll_create(1);
            macros::ASSERT(epoll_fd_ != -1, "epoll_create() failed", macros::SOURCE_LOCATION(), errno);
            macros::ASSERT(listener_socket_.connect("", interface, port, Listening::YES, Blocking::NO) != -1,
                "Listener socket creation failed.", macros::SOURCE_LOCATION(), errno);
            macros::ASSERT(add_to_epoll_list(&listener_socket_) != -1, "epoll_ctl() failed", macros::SOURCE_LOCATION(), errno);
        }

        auto poll() noexcept -> void 
        {

        }

        auto send_and_recv() noexcept -> void
        {

        }




    };

}
