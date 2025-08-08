#include <ranges>
#include <utility>

#include "order_book.hpp"

// reverse vector ordering to reduce copies
void engine::OrderBookVector::add_order(Side side, Price price, Volume volume) {
    if (side == Side::bid)
        return add_order(bid_levels_, price, volume, std::less<Price>());
    else
        return add_order(ask_levels_, price, volume, std::greater<Price>());
}

template <typename T, typename Compare>
void engine::OrderBookVector::add_order(T& levels, Price price, Volume volume, Compare comp) {
    auto it = std::ranges::find_if(levels, price,
        [comp](const auto& p, Price price) { return comp(p.first, price); });
    
    if (it != levels.end() && it->first == price) // [[attribute]]
        it->second += volume;
    else
        levels.insert(it, {price, volume});

}

std::pair<common::Price, common::Volume> engine::OrderBookVector::get_best_prices() const {
    return std::make_pair(bid_levels_.rbegin()->first, ask_levels_.rbegin()->first);
}