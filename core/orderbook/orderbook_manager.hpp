#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <atomic>

#include "common/queues/MessageQueues.hpp"
#include "network/socket/feed_handler.hpp"
#include "orderbook/L2/orderbook.hpp"

class OrderbookManager
{
public:   
    OrderbookManager();
    ~OrderbookManager();

    OrderbookManager(const OrderbookManager&) = delete;
    OrderbookManager& operator=(const OrderbookManager&) = delete;
    OrderbookManager(OrderbookManager&&) = default;
    OrderbookManager& operator=(OrderbookManager&&) = default;

private:
    std::unordered_map<std::string, L2::OrderBook> orderbooks_;
    std::vector<std::unique_ptr<std::thread>> managers_;

    std::atomic<bool> running_{ false };
};