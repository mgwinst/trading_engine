#include "tcp_socket.h"

using namespace network;

Socket::Socket(const SocketConfig& socket_config) {}
Socket::Socket(Socket&& socket) {}
Socket& Socket::operator=(Socket&& socket) {}
Socket::~Socket() { close(_fd); }

bool Socket::set_non_blocking() {
    const auto flags = fcntl(_fd, F_GETFL, 0); // this returns the flags associated with fd currently
    if (flags == -1) return false;
    if (flags & O_NONBLOCK) return true;
    return (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) == 0);
}

// implicit reinterpret_cast<void*> when passing to const void* optval in C++
bool Socket::disable_nagle() {
    int32_t flag = 1;
    return (setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag)) == 0);
}

bool Socket::set_so_time_stamp() {
    int32_t flag = 1;
    return (setsockopt(_fd, SOL_SOCKET, SO_TIMESTAMP, &flag, sizeof(flag)) == 0);
}

bool Socket::would_block() {
    return (errno == EWOULDBLOCK || errno == EINPROGRESS);
}

bool Socket::set_unicast_ttl(int32_t ttl) {
    return (setsockopt(_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == 0);
}

bool Socket::set_mcast_ttl(int32_t ttl) {
    return (setsockopt(_fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) == 0);
}

bool Socket::join_mcast(const std::string& ip) {
    const ip_mreq mreq{{inet_addr(ip.c_str())}, {htonl(INADDR_ANY)}};
    return (setsockopt(_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) != -1);
}




