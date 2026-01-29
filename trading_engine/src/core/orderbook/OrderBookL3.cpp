#include "OrderBookL3.hpp"

// enforce with templates that I am not using uninitialized msg fields for msg type (this is currently danger.)
void OrderBookL3::process_exchange_message(const Message& msg) noexcept
{
    switch (static_cast<char>(msg.msg_type)) {
        case 'A': return add_order(msg.order_id, msg.price, msg.qty, msg.side); 
        case 'X': return cancel_order(msg.order_id, msg.qty);
        case 'D': return delete_order(msg.order_id);
        case 'U': return replace_order(msg.order_id, msg.new_order_id, msg.price, msg.qty);
        case 'E':
        case 'C': return execute_order(msg.order_id, msg.qty);

        default: return;
    }
}

uint32_t OrderBookL3::register_order(uint64_t order_id, uint32_t price, uint32_t qty, uint8_t side) noexcept
{
    order_pool.push_back(Order{ 
        .next = -1, 
        .prev = -1, 
        .order_id = order_id, 
        .price = price, 
        .qty_remaining = qty, 
        .qty_original = qty, 
        .side = side });

    auto idx = order_pool.size() - 1;
    order_index_by_id[order_id] = idx;

    return idx;
}

void OrderBookL3::deregister_order(uint32_t index) noexcept
{
    if (index >= order_pool.size()) return;

    order_index_by_id.erase(order_pool[index].order_id);

    if (index != order_pool.size() - 1) [[likely]] {
        std::swap(order_pool[index], order_pool.back());
        order_index_by_id[order_pool[index].order_id] = index;
    }

    order_pool.pop_back();
}

void OrderBookL3::add_order(uint64_t order_id, uint32_t price, uint32_t qty, uint8_t side) noexcept
{
    auto order_idx = register_order(order_id, price, qty, side);

    auto& level = get_price_level(order_idx);

    queue_order(level, order_idx);   
}

void OrderBookL3::delete_order(uint64_t order_id) noexcept
{
    auto order_idx = order_index_by_id[order_id];

    dequeue_order(order_idx);

    deregister_order(order_idx);
}

void OrderBookL3::cancel_order(uint64_t order_id, uint32_t qty) noexcept
{
    auto order_idx = order_index_by_id[order_id];
    auto& order = order_pool[order_idx];

    auto removed = std::min(order.qty_remaining, qty);

    get_price_level(order).total_qty -= removed;
    order.qty_remaining -= removed;

    if (order.qty_remaining == 0) {
        unlink(order_idx);
        deregister_order(order_idx);
    }
}

// replace -> delete + add
void OrderBookL3::replace_order(uint64_t old_order_id, uint64_t new_order_id, uint32_t price, uint32_t qty) noexcept
{
    auto order_idx = order_index_by_id[old_order_id];
    auto& order = order_pool[order_idx];

    order_index_by_id.erase(order.order_id);
    order_index_by_id.emplace(new_order_id, order_idx);

    order.order_id = new_order_id;
    order.price = price;
    order.qty_original = qty;
    order.qty_remaining = qty;

    dequeue_order(order_idx);
    
    auto& level = get_price_level(order);
    
    queue_order(get_price_level(order), order_idx);   
}

void OrderBookL3::execute_order(uint64_t order_id, uint32_t qty) noexcept
{
    auto order_idx = order_index_by_id[order_id];
    auto& order = order_pool[order_idx];

    if (order.qty_remaining <= qty) {
        dequeue_order(order_idx);
        deregister_order(order_idx);
    } else {
        get_price_level(order).total_qty -= qty;
        order.qty_remaining -= qty;
    }
}

void OrderBookL3::queue_order(Level& level, uint32_t order_idx) noexcept
{
    link_tail(level, order_idx);

    level.order_count += 1;
    level.total_qty += order_pool[order_idx].qty_original;
}

void OrderBookL3::dequeue_order(uint32_t order_idx) noexcept
{
    auto& order = order_pool[order_idx];
    auto& level = get_price_level(order);

    level.order_count -= 1;
    level.total_qty -= std::min(level.total_qty, order.qty_remaining);

    unlink(order_idx);
}

void OrderBookL3::link_tail(Level& level, uint32_t order_idx) noexcept
{
    auto& order = order_pool[order_idx];

    if (level.order_count == 0) {
        level.head = order_idx;
        level.tail = order_idx;
    } else {
        order.prev = level.tail;
        level.tail = order_idx;
    }
}

void OrderBookL3::unlink(uint32_t order_idx) noexcept
{
    auto& order_to_remove = order_pool[order_idx];

    // check to make sure prev and next are valid orders (not -1)
    auto& prev_order = order_pool[order_to_remove.prev];
    auto& next_order = order_pool[order_to_remove.next];

    prev_order.next = order_to_remove.next;
    next_order.prev = order_to_remove.prev;
}

Level& OrderBookL3::get_price_level(Order& order) noexcept
{
    auto& levels = order.side == 'B' ? bids : asks;
    return levels[order.price];
}

Level& OrderBookL3::get_price_level(uint32_t order_idx) noexcept
{
    auto& order = order_pool[order_idx];
    auto& levels = order.side == 'B' ? bids : asks;
    return levels[order.price];
}