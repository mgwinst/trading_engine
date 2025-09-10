#include <ranges>
#include <utility>
#include <algorithm>

#include "orderbook.hpp"

namespace rng = std::ranges;

namespace L2
{
    OrderBook::OrderBook()
    {
    }

    OrderBook::~OrderBook()
    {
    }

    void OrderBook::add_order(const Price price, const Volume volume, const Side side) noexcept
    {
        if (side == Side::Bid)
            return add_order(bid_levels_, price, volume, std::less<Price>());
        else
            return add_order(ask_levels_, price, volume, std::greater<Price>());
    }

    template <typename T, typename Compare>
    void OrderBook::add_order(T& levels, const Price price, const Volume volume, Compare comp) noexcept
    {
        auto it = rng::find_if_not(rng::reverse_view(levels), [comp, price](auto& p){ return comp(price, p.first); });
        
        if (it != levels.rend() && it->first == price) [[likely]] {
            it->second += volume;
        } else {
            levels.insert(it.base(), {price, volume});
        }
    }

} // namespace L2
