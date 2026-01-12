#include "orderbook_manager.hpp"
#include "symbol_directory.hpp"
#include "../network/protocol/message.hpp"
#include "../common/queues/MessageQueuePool.hpp"
#include "../common/thread_utils.hpp"
#include "../common/CoreSet.hpp"

OrderbookManager::OrderbookManager() : running_{ true }
{
    auto& msg_queues = MessageQueuePool<Message>::instance();
    msg_queues.add_queues(tickers); // check for duplicate queues?

    // auto symbols = SymbolDirectory::instance();

    for (auto symbol : symbols) {
        orderbooks_.emplace(symbol, OrderBook{});

        auto orderbook_worker = [this, ticker, &msg_queues] {
            while (running_.load()) {

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