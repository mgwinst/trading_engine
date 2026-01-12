#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <atomic>
#include <thread>

#include "orderbook.hpp"
#include "../network/protocol/message.hpp"
#include "../common/CoreSet.hpp"

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
    std::unordered_map<Symbol, OrderBook> orderbooks_;
    std::vector<CoreID> claimed_cores_;
};