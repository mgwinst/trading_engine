#include "sys/epoll.h"

#include "network/socket/raw_socket.hpp"
#include "network/socket/socket_utils.hpp"
#include "network/socket/tpacket.hpp"
#include "common/macros.hpp"
#include "common/memory.hpp"

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
        delete[] ring_.rd;

        if (fd_)
            close(fd_);
    }

    void RawSocket::read()
    {
        static std::size_t block_idx{ 0 };

        for (std::size_t block_iter = 0; block_iter < ring_.req.tp_block_nr; block_iter++) {
            tpacket_block_desc* block_desc = reinterpret_cast<tpacket_block_desc *>(ring_.rd[block_idx].block_ptr);
            if (!is_block_readable(block_desc)) {
                block_idx = (block_idx + 1) & (ring_.req.tp_block_nr - 1);
                continue;
            }
            
            process_block(block_desc);
            flush_block(block_desc);
            block_idx = (block_idx + 1) & (ring_.req.tp_block_nr - 1);

            block_iter = 0;
        }
    }

} // namespace network
