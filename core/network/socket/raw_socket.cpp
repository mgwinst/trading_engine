#include "raw_socket.hpp"
#include "socket_utils.hpp"
#include "common/macros.hpp"
#include "common/memory.hpp"
#include "tpacket.hpp"

namespace network
{
    RawSocket::RawSocket(std::string_view interface)
    {
        if (!interface_exists(interface.data()))
            error_exit("interface doesn't exist");

        int32_t socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

        if (socket_fd == -1)
            error_exit("socket()");

        configure_ring(socket_fd, ring_);

        if (set_non_blocking(socket_fd))
            error_exit("set_non_blocking()");

        if (bind_to_interface(socket_fd, interface.data()))
            error_exit("bind_to_interface()");

        fd_ = socket_fd;
    }

    RawSocket::~RawSocket()
    {
        munmap(ring_.map, ring_.req.tp_block_size * ring_.req.tp_block_nr);
        std::free(ring_.rd);

        if (fd_)
            close(fd_);
    }

} // namespace network
