#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

#include "common/queues/MessageQueues.hpp"
#include "network/socket/feed_handler.hpp"
#include "orderbook/L2/orderbook.hpp"

class OrderbookManager
{
public:   
    OrderbookManager(const std::vector<std::string>& tickers);

    OrderbookManager(const OrderbookManager&) = delete;
    OrderbookManager& operator=(const OrderbookManager&) = delete;
    OrderbookManager(OrderbookManager&&) = default;
    OrderbookManager& operator=(OrderbookManager&&) = default;
    ~OrderbookManager() = default;

private:
    std::unordered_map<std::string, L2::OrderBook> orderbooks;
    std::vector<std::unique_ptr<std::thread>> managers;

};