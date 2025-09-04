#include "raw_socket.hpp"
#include "socket_utils.hpp"
#include "common/macros.hpp"
#include "common/memory.hpp"

namespace network
{
    RawSocket::RawSocket()
    {

    }

    RawSocket::RawSocket(std::string_view interface)
    {
        // make sure userspace buffer is pinned and aligned...

        if (!interface_exists(interface.data()))
            error_exit("interface doesn't exist");
        
        int socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

        if (socket_fd == -1)
            error_exit("socket()");
        
        if (set_non_blocking(socket_fd)) {
            close(socket_fd);
            error_exit("set_non_blocking()");
        }

        if (set_timestamp(socket_fd, TimestampType::RX_SOFTWARE_AND_HARDWARE)) {
            close(socket_fd);
            error_exit("set_timestamp()");
        }

        if (bind_to_interface(socket_fd, interface.data())) {
            close(socket_fd);
            error_exit("bind_to_interface()");
        }

        fd_ = socket_fd;
    }

    RawSocket::~RawSocket()
    {
        if (fd_) close(fd_);
    }

} // end of namespace
