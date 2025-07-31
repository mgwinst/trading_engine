#include "socket.hpp"

using namespace network::utilities;

Socket::Socket(int32_t fd) noexcept : fd_{fd} {}

Socket::Socket(Socket&& socket) noexcept
{
    if (this != &socket) {
        fd_ = socket.fd_;
        socket.fd_ = -1;
    }
}

Socket& Socket::operator=(Socket&& socket) noexcept
{
    if (this != &socket) {
        if (fd_ != -1) close(fd_);
        fd_ = socket.fd_;
        socket.fd_ = -1;
    }
    return *this;
}

Socket::~Socket() noexcept 
{ 
    close(fd_);
}

auto Socket::get_fd() const noexcept
{ 
    return fd_;
}

auto Socket::release_fd() noexcept
{
    auto old_fd = fd_;
    fd_ = -1;
    return old_fd;
}
