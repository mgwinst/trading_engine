#include "feed_handler.hpp"

namespace network
{
    FeedHandler::FeedHandler()
    {
        epoll_fd_ = epoll_create(1);

        if (epoll_fd_ == -1)
            error_exit("epoll_create()");
    }

    FeedHandler::~FeedHandler()
    {
        if (epoll_fd_)
            close(epoll_fd_);
    }

    void FeedHandler::add_to_epoll_list(std::shared_ptr<RawSocket>& socket)
    {
        epoll_event ev{ .events = EPOLLIN | EPOLLET, .data = {reinterpret_cast<void *>(socket.get())} };

        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket->fd_, &ev))
            error_exit("epoll_ctl()");
    }

    void FeedHandler::start_rx(SPSCQueue<uint8_t>& buffer)
    {
        std::size_t block_idx{ 0 };
        
        running_.store(true);

        while (running_) {
            for (std::size_t i = 0; i < rx_socket_->ring_.req.tp_block_nr; i++) {
                tpacket_block_desc* block_desc = reinterpret_cast<tpacket_block_desc *>(rx_socket_->ring_.rd[block_idx].iov_base);
                if (!is_block_readable(block_desc)) {
                    block_idx = (block_idx + 1) % rx_socket_->ring_.req.tp_block_nr;
                    continue;
                }
                
                process_block(block_desc, buffer);
                flush_block(block_desc);
                block_idx = (block_idx + 1) % rx_socket_->ring_.req.tp_block_nr;

                i = 0;
            }

            int32_t event = epoll_wait(epoll_fd_, events_, 1, -1);
            if (event == -1) {
                error_exit("epoll_wait()")
            }
        }
    }

    void FeedHandler::stop_rx()
    {
        running_.store(false);
    }
} // end of namespace