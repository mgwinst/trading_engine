#pragma once

#include <cstdint>
#include <map>

#include "common/types.h"

namespace engine {
    
    struct OrderBookMap {

        using Order_ID = common::Order_ID;
        using Ticker_ID = common::Ticker_ID;
        using Side = common::Side;
        using Price = common::Price;
        using Volume = common::Volume;
        using Quantity = common::Quantity;
        using Priority = common::Priority;

        std::map<Price, Volume, std::greater<Price>> bid_levels_; 
        std::map<Price, Volume, std::less<Price>> ask_levels_; 

        template <typename T>
        T::iterator add_order(T& levels, Order_ID order_id, Side side, Price price, Volume volume, Quantity quantity);
        
        template <typename T>
        void delete_order(T::iterator it, T& levels, Order_ID order_id, Price price, Volume volume);

    };
    

    


}
