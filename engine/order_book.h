#pragma once

#include <cstdint>
#include <map>

namespace engine {
    
    enum class Side : uint8_t { bid, ask };

    using Order_ID  = uint64_t;
    using Ticker_ID = uint64_t;
    using Price = uint64_t;
    using Volume = uint64_t;
    using Quantity = uint64_t;
    using Priority = uint64_t;

    struct OrderBook {

        std::map<Price, Volume, std::greater<Price>> bid_levels_; 
        std::map<Price, Volume, std::less<Price>> ask_levels_; 

        template <typename T>
        T::iterator add_order(T& levels, Order_ID order_id, Side side, Price price, Volume volume, Quantity quantity);
        
        template <typename T>
        void delete_order(T::iterator it, T& levels, Order_ID order_id, Price price, Volume volume);


    };
    

    


}
