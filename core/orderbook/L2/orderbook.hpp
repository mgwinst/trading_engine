#pragma once

#include <vector>
#include <utility>
#include <unordered_map>

#include "orderbook/orderbook_types.hpp"

namespace L2
{
    class OrderBook
    {
    public:
        OrderBook();
        OrderBook(const OrderBook&) = delete;
        OrderBook& operator=(const OrderBook&) = delete;
        OrderBook(OrderBook&&) = default;
        OrderBook& operator=(OrderBook&&) = default;
        ~OrderBook();
        
        void process_order(Action action, const Price, const Volume, const Side) noexcept;
    
        std::vector<Price> bid_prices() const noexcept;
        std::vector<Price> ask_prices() const noexcept;
        std::pair<Price, Price> best_prices() const noexcept;

    private:
        std::vector<std::pair<Price, Volume>> bid_levels_;
        std::vector<std::pair<Price, Volume>> ask_levels_;
        // std::unordered_map<size_t, Order> orders_;

        template <typename T, typename Compare>
        void process_order(Action action, T& levels, const Price price, const Volume volume, Compare comp) noexcept;

        template <typename T>
        std::vector<Price> get_prices(T &levels) const noexcept;
    };

} // namespace orderbook
