#pragma once

#include <vector>

#include "common/types.hpp"

namespace engine {

    struct OrderBook {

        using Order_ID = common::Order_ID;
        using Side = common::Side;
        using Price = common::Price;
        using Volume = common::Volume;
        using Priority = common::Priority;

        std::vector<std::pair<Price, Volume>> bid_levels_;
        std::vector<std::pair<Price, Volume>> ask_levels_;

        void add_order(Side side, Price price, Volume volume);

        template <typename T, typename Compare>
        void engine::OrderBook::add_order(T &levels, Price price, Volume volume, Compare comp);

        std::pair<Price, Volume> get_best_prices() const;
    };

}