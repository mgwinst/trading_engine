#pragma once

#include <vector>

#include "orderbook/types.hpp"

namespace L2
{
    class OrderBook
    {
    public:
        OrderBook();
        OrderBook(const OrderBook&) = delete;
        OrderBook& operator=(const OrderBook&) = delete;
        OrderBook(OrderBook&&) = delete;
        OrderBook& operator=(OrderBook&&) = delete;
        ~OrderBook();
        
        void add_order(const Price, const Volume, const Side) noexcept;
        void modify_order() noexcept;
        void cancel_order() noexcept;
    
        BestPrice best_prices() noexcept;
        std::vector<Price> bid_prices() noexcept;
        std::vector<Price> ask_prices() noexcept;

    // private:
        std::vector<std::pair<Price, Volume>> bid_levels_;
        std::vector<std::pair<Price, Volume>> ask_levels_;

        template <typename T, typename Compare>
        void add_order(T& levels, const Price price, const Volume volume, Compare comp) noexcept;
    };

} // namespace orderbook