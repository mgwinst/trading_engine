#include "orderbook_manager.hpp"
#include "../network/protocol/message.hpp"
#include "../common/queues/MessageQueuePool.hpp"
#include "../common/thread_utils.hpp"
#include "../common/CoreSet.hpp"

// this should not be in constructor (running in ctor?!?!?!)
OrderbookManager::OrderbookManager(const std::vector<std::string>& tickers) : running_{ true }
{
    auto& msg_queues = MessageQueuePool<Message>::instance();
    msg_queues.add_queues(tickers); // check for duplicate queues?

    for (auto ticker : tickers) {
        orderbooks_.emplace(ticker, OrderBook{});

        auto orderbook_worker = [this, ticker, &msg_queues] {
            while (running_.load()) {

                Message msg{};
                while (msg_queues[ticker]->try_pop(msg)) {

                    switch (msg.index()) {
                        case 3: {
                            auto& add_order = std::get<3>(msg);
                            orderbooks_.at(ticker).process_order(Action::Add, add_order.price, add_order.num_shares, add_order.side);
                            break;
                        }

                        case 4: {
                            auto& cancel_order = std::get<4>(msg);
                            break;
                        }

                        case 5: {
                            auto& delete_order = std::get<5>(msg);
                            break;
                        }

                        case 6: {
                            auto& delete_order = std::get<6>(msg);
                            break;
                        }

                        default:
                            break;
                    }
                }
            }
        };

        while (true) {
            auto core_id = CoreSet::instance().claim_core();
            claimed_cores_.push_back(core_id);

            auto orderbook_thread = common::create_and_pin_thread(core_id, orderbook_worker);

            if (!orderbook_thread.has_value()) {
                auto core_id = claimed_cores_.back();
                CoreSet::instance().release_core(core_id);
                claimed_cores_.pop_back();
            } else {
                orderbook_threads_.push_back(std::move(*orderbook_thread));
                break;
            }
        }
    }
}

OrderbookManager::~OrderbookManager()
{
    running_.store(false);

    for (const auto& core_id : claimed_cores_) {
        CoreSet::instance().release_core(core_id);
    }
}