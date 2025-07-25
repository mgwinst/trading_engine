#include "algorithm"

#include "order_book_vector.h"

// reverse vector ordering to reduce copies
void engine::OrderBookVector::add_order(Side side, Price price, Volume volume) {
    if (side == Side::bid)
        return add_order(bid_levels_, price, volume, std::less<Price>());
    else
        return add_order(ask_levels_, price, volume, std::greater<Price>());
}

template <typename T, typename Compare>
void engine::OrderBookVector::add_order(T& levels, Price price, Volume volume, Compare comp) {
    auto it = std::lower_bound(levels.begin(), levels.end(), price,
        [comp](const auto& p, Price price) { return comp(p.first, price); });
    
    if (it != levels.end() && it->first == price)
        it->second += volume;
    else
        levels.insert(it, {price, volume});

}

auto engine::OrderBookVector::get_best_prices() const {
    return {bid_levels_.rbegin()->first, ask_levels_.rbegin()->first};
}