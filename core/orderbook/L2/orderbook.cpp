#include <ranges>
#include <utility>
#include <algorithm>

#include "orderbook.hpp"

namespace rng = std::ranges;

OrderBook::OrderBook()
{
    // bid_levels_.resize(MAX_LEVELS); 
    // ask_levels_.resize(MAX_LEVELS);
};

void OrderBook::process_order(Action action, const Price price, const Volume volume, const Side side) noexcept
{
    if (side == 'B')
        return process_order(action, bid_levels_, price, volume, std::less<Price>());
    else
        return process_order(action, ask_levels_, price, volume, std::greater<Price>());
}

template <typename T, typename Compare>
void OrderBook::process_order(Action action, T& levels, const Price price, const Volume volume, Compare comp) noexcept
{
    auto it = rng::find_if_not(rng::reverse_view(levels), [comp, price](auto& p){ return comp(price, p.first); });
    
    if (it != levels.rend() && it->first == price) [[likely]] {
        // switch (Action::___)

        if (action == Action::Add)
            it->second += volume;
        else {
            it->second -= volume;
            if (it->second <= 0)
                levels.erase(it.base());
        }
    } else {
        levels.insert(it.base(), {price, volume});
    }
}