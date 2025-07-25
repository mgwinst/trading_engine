#include "order_book_map.h"

template <typename T>
T::iterator engine::OrderBookMap::add_order(T& levels, Order_ID order_id, Side side, Price price, Volume volume, Quantity quantity) {
    auto [it, inserted] = levels.try_emplace(price, volume);
    if (inserted == false) 
        it->second += volume;
    return it;
}

template <typename T>
void engine::OrderBookMap::delete_order(T::iterator it, T& levels, Order_ID order_id, Price price, Volume volume) {
    it->second -= volume;
    if (it->second <= 0)
        levels.erase(it);
}