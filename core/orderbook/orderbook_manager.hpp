#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <set>

#include "common/queues/MessageQueues.hpp"
#include "network/socket/feed_handler.hpp"
#include "orderbook/L2/orderbook.hpp"

class OrderbookManager
{
public:   
    OrderbookManager(const std::vector<std::string>& tickers);
    ~OrderbookManager();

    OrderbookManager(const OrderbookManager&) = delete;
    OrderbookManager& operator=(const OrderbookManager&) = delete;
    OrderbookManager(OrderbookManager&&) = delete;
    OrderbookManager& operator=(OrderbookManager&&) = delete;

private:
    std::atomic<bool> running_{ false };
    std::vector<std::jthread> orderbook_threads_;
    std::unordered_map<std::string, L2::OrderBook> orderbooks_;
    std::vector<CoreID> claimed_cores_;
};