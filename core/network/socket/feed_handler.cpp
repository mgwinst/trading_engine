#include "network/socket/feed_handler.hpp"
#include "common/thread_utils.hpp"
#include "common/json_parser.hpp"
#include "common/CoreSet.hpp"

namespace network
{
    FeedHandler::FeedHandler(std::shared_ptr<RawSocket>& socket)
    {
        epoll_fd_ = epoll_create(1);

        if (epoll_fd_ == -1)
            error_exit("epoll_create()");

        add_to_epoll_list(socket);
    }

    FeedHandler::~FeedHandler()
    {
        if (running_.load())
            stop_rx();

        if (epoll_fd_)
            close(epoll_fd_);
    }

    void FeedHandler::add_to_epoll_list(std::shared_ptr<RawSocket>& socket)
    {
        rx_socket_ = socket;     

        epoll_event ev{ .events = EPOLLIN | EPOLLET, .data = {reinterpret_cast<void *>(socket.get())} };

        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket->fd_, &ev))
            error_exit("epoll_ctl()");
    }

    void FeedHandler::start_rx()
    {
        running_.store(true);
        
        auto rx_loop = [&] {
            while (running_.load()) {
                rx_socket_->read();
                
                int32_t event = epoll_wait(epoll_fd_, events_, 1, -1);
                if (event == -1) {
                    error_exit("epoll_wait()")
                }
            }
        };

        while (true) {
            auto core_id = CoreSet::instance().claim_core();
            claimed_core_ = core_id;

            auto rxt = common::create_and_pin_thread(core_id, rx_loop);

            if (!rxt.has_value()) {
                CoreSet::instance().release_core(claimed_core_);
                claimed_core_ = -1;
            } else {
                rx_thread_ = std::move(*rxt);
                break;
            }
        }
    }

    void FeedHandler::stop_rx()
    {
        running_.store(false);

        CoreSet::instance().release_core(claimed_core_);
    }

} // namespace network