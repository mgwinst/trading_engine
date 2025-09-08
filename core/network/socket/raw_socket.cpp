#include "raw_socket.hpp"
#include "socket_utils.hpp"
#include "common/macros.hpp"
#include "common/memory.hpp"
#include "tpacket.hpp"

namespace network
{
    RawSocket::RawSocket(std::string_view interface)
    {
        fd_ = setup_socket(ring_, interface);
    }

    RawSocket::~RawSocket()
    {
        munmap(ring_.map, ring_.req.tp_block_size * ring_.req.tp_block_nr);
        std::free(ring_.rd);

        if (fd_)
            close(fd_);
    }
} // end of namespace
