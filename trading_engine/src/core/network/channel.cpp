#include "channel.hpp"
#include "socket.hpp"
#include "../common/thread_utils.hpp"
#include "../common/cores.hpp"

namespace network
{
    void Channel::add_to_epoll_list(std::shared_ptr<RawSocket>& socket)
    {
        rx_socket_ = socket;     

        epoll_event ev{.events = EPOLLIN | EPOLLET, .data = {reinterpret_cast<void *>(socket.get())} };

        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket->get_fd(), &ev))
            error_exit("epoll_ctl()");
    }

    void Channel::start_rx()
    {
        running_.store(true);
        
        auto event_loop = [&] {
            while (running_.load()) {
                on_event();
                
                int32_t event = epoll_wait(epoll_fd_, events_, 1, -1);
                if (event == -1)
                    error_exit("epoll_wait()")
            }
        };

        while (true) {
            auto core_id = CoreSet::instance().claim_core();
            claimed_core_ = core_id;

            auto rxt = common::create_and_pin_thread(core_id, event_loop);

            if (!rxt.has_value()) {
                CoreSet::instance().release_core(claimed_core_);
                claimed_core_ = -1;
            } else {
                rx_thread_ = std::move(*rxt);
                break;
            }
        }
    }

    void Channel::stop_rx()
    {
        running_.store(false);

        CoreSet::instance().release_core(claimed_core_);
    }

} // namespace network