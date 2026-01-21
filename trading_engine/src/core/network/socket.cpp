#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>

#include "socket.hpp"
#include "packet.hpp"
#include "socket_utils.hpp"
#include "../common/macros.hpp"

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
        munmap(ring_.buffer, ring_.req.tp_block_size * ring_.req.tp_block_nr);

        if (fd_)
            close(fd_);
    }

    void RawSocket::read()
    {
        static auto block_idx = 0uz;

        for (auto block_iter = 0uz; block_iter < ring_.blocks.size(); block_iter++) {
            if (!is_block_readable(ring_.blocks[block_idx])) {
                block_idx = (block_idx + 1) & (ring_.blocks.size() - 1);
                continue;
            }

            process_block(ring_.blocks[block_idx]);
            flush_block(ring_.blocks[block_idx]);
            block_idx = (block_idx + 1) & (ring_.blocks.size() - 1);

            block_iter = 0;
        }
    }

} // namespace network
