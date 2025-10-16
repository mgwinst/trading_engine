#include <filesystem>

#include "orderbook/orderbook_manager.hpp"
#include "parser/msg_types.hpp"
#include "common/json_parser.hpp"
#include "common/thread_utils.hpp"
#include "common/CoreSet.hpp"

OrderbookManager::OrderbookManager(const std::vector<std::string>& tickers) : running_{ true }
{
    auto& msg_queues = MessageQueues<MessageVariant>::instance();
    msg_queues.add_queues(tickers); // check for duplicate queues?

    for (auto ticker : tickers) {
        orderbooks_.emplace(ticker, OrderBook{});

        auto orderbook_worker = [this, ticker, &msg_queues] {
            while (running_.load()) {

                MessageVariant msg{};
                while (msg_queues[ticker]->try_pop(msg)) {

                    switch (msg.index()) {
                        case 3: {
                            auto& add_order = std::get<3>(msg);
                            orderbooks_.at(ticker).process_order(Action::Add, add_order.price, add_order.num_shares, add_order.side);
                            break;
                        }
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