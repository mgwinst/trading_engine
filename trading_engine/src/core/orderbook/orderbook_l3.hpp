#pragma once

#include <cstdint>
#include <cstdio>
#include <cwchar>
#include <optional>
#include <vector>

#include <boost/unordered/unordered_flat_map.hpp>

#include "../network/message.hpp"

class OrderbookStats;

struct Order
{
    int32_t next;
    int32_t prev;
    uint64_t order_id;
    uint32_t price;
    uint32_t qty_remaining;
    uint32_t qty_original; // useful for modify semantics, whether to update queue position
    uint8_t side;
};

struct Level
{
    int32_t head;
    int32_t tail;
    uint32_t total_qty;
    uint32_t order_count;
};

class OrderBookL3
{
public:
    OrderBookL3() noexcept :
        bids(NUM_TICKS),
        asks(NUM_TICKS) {}

    OrderBookL3(const OrderBookL3&) noexcept = delete;
    OrderBookL3& operator=(const OrderBookL3&) noexcept = delete;
    OrderBookL3(OrderBookL3&&) noexcept = default;
    OrderBookL3& operator=(OrderBookL3&&) noexcept = default;
    ~OrderBookL3() noexcept = default;
  
    void process_exchange_message(const Message& msg) noexcept;

private:
    static constexpr std::size_t NUM_TICKS = 5e5;

    std::vector<Level> bids;
    std::vector<Level> asks;
    std::vector<Order> order_pool;
    boost::unordered_flat_map<uint64_t, uint32_t> order_index_by_id;

    uint32_t register_order(uint64_t order_id, uint32_t price, uint32_t qty, uint8_t side) noexcept;
    void deregister_order(uint32_t index) noexcept;

    void add_order(uint64_t order_id, uint32_t price, uint32_t qty, uint8_t side) noexcept;
    void delete_order(uint64_t order_id) noexcept;
    void cancel_order(uint64_t order_id, uint32_t qty) noexcept;
    void cancel_order(uint64_t order_id, uint32_t qty, int x) noexcept;
    void replace_order(uint64_t old_order_id, uint64_t new_order_id, uint32_t price, uint32_t qty) noexcept;
    void execute_order(uint64_t order_id, uint32_t qty) noexcept;

    void queue_order(Level& price_level, uint32_t order_idx) noexcept;
    void dequeue_order(uint32_t order_idx) noexcept;

    void link_tail(Level& price_level, uint32_t order_idx) noexcept;
    void unlink(uint32_t order_idx) noexcept;

    bool get_order_idx(uint64_t order_id, uint32_t& out_idx) noexcept;
    Level& get_price_level(Order& order) noexcept;
    Level& get_price_level(uint32_t order_idx) noexcept;

    friend class OrderbookStats;
};
