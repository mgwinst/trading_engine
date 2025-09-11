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

    std::vector<Price> OrderBook::bid_prices() const noexcept
    {
        return get_prices(bid_levels_);
    }

    std::vector<Price> OrderBook::ask_prices() const noexcept
    {
        return get_prices(ask_levels_);
    }

    template <typename T>
    std::vector<Price> OrderBook::get_prices(T& levels) const noexcept
    {
        std::vector<Price> prices;

        for (auto& x : levels)
            prices.push_back(x.first);

        return prices;
    }

    // throw on no buy or sell orders
    std::pair<Price, Price> OrderBook::best_prices() const
    {
        auto best_bid = bid_levels_.back().first;
        auto best_ask = ask_levels_.back().first;

        return {best_bid, best_ask};
        
    }

    void OrderBook::process_order(Action action, const Price price, const Volume volume, const Side side) noexcept
    {
        if (side == Side::Bid)
            return process_order(action, bid_levels_, price, volume, std::less<Price>());
        else
            return process_order(action, ask_levels_, price, volume, std::greater<Price>());
    }

    template <typename T, typename Compare>
    void OrderBook::process_order(Action action, T& levels, const Price price, const Volume volume, Compare comp) noexcept
    {
        auto it = rng::find_if_not(rng::reverse_view(levels), [comp, price](auto& p){ return comp(price, p.first); });
        
        if (it != levels.rend() && it->first == price) [[likely]] {
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

} // namespace L2
