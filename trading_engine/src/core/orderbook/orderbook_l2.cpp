#include <ranges>
#include <algorithm>

#include "orderbook_l2.hpp"

namespace rng = std::ranges;

OrderBookL2::OrderBookL2()
{
    // bid_levels_.resize(NUM_TICKS); 
    // ask_levels_.resize(NUM_TICKS);
};

void OrderBookL2::process_exchange_message(const Message& msg) noexcept
{

}

void OrderBookL2::process_order(const uint8_t order_type, const uint32_t price, const uint32_t qty, const uint8_t side) noexcept
{
    if (side == 'B')
        return process_order(order_type, bid_levels_, price, qty, std::less<Price>());
    else
        return process_order(order_type, ask_levels_, price, qty, std::greater<Price>());
}

template <typename T, typename Compare>
void OrderBookL2::process_order(const uint8_t order_type, T& levels, const uint32_t price, const uint32_t qty, Compare comp) noexcept
{
    auto it = rng::find_if_not(rng::reverse_view(levels), [comp, price](auto& p){ return comp(price, p.first); });
    
    if (it != levels.rend() && it->first == price) [[likely]] {
        // switch (Action::___)

        if (order_type == 'A') // add
            it->second += qty;
        else {
            it->second -= qty;
            if (it->second <= 0)
                levels.erase(it.base());
        }
    } else {
        levels.insert(it.base(), {price, qty});
    }
}