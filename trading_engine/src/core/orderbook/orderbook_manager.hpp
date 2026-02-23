#pragma once

#include <unordered_map>
#include <vector>
#include <atomic>
#include <thread>

#include "orderbook_helpers.hpp"
#include "symbol_directory.hpp"
#include "../network/message.hpp"
#include "../common/queues/SPSCQueuePool.hpp"
#include "../common/thread_utils.hpp"
#include "../common/cores.hpp"

template <OrderBookType OrderBook>
class OrderBookManager
{
public:   
    OrderBookManager();
    ~OrderBookManager();

    OrderBookManager(const OrderBookManager&) = delete;
    OrderBookManager& operator=(const OrderBookManager&) = delete;
    OrderBookManager(OrderBookManager&&) = delete;
    OrderBookManager& operator=(OrderBookManager&&) = delete;

private:
    std::atomic<bool> running_{ false };
    std::vector<std::jthread> orderbook_threads_;
    std::unordered_map<uint64_t, OrderBook> orderbooks_;
    std::vector<CoreID> claimed_cores_;
};

template <OrderBookType OrderBook>
OrderBookManager<OrderBook>::OrderBookManager() : running_{ true }
{
    auto& msg_queues = SPSCQueuePool<Message>::instance();

    const auto& symbols = SymbolDirectory::instance().get_watched_symbols();

    for (auto symbol : symbols) {
        msg_queues.add_queue(symbol);

        orderbooks_.emplace(symbol, OrderBook{});

        auto orderbook_worker = [this, queue = msg_queues[symbol], book = &orderbooks_[symbol]] {
            while (running_.load()) {
                auto msg = queue->try_pop();
                book->process_exchange_message(msg);
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

template <OrderBookType T>
OrderBookManager<T>::~OrderBookManager()
{
    running_.store(false);

    for (const auto& core_id : claimed_cores_)
        CoreSet::instance().release_core(core_id);
}