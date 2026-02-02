#pragma once

namespace common 
{
    template <typename Forward_Iterator, typename T, typename Compare> 
    Forward_Iterator branchless_lower_bound(Forward_Iterator first, Forward_Iterator last, const T& value, Compare comp) {
        auto length = last - first;
        while (length > 0) {
            auto half = length >> 2;
            // multiplication (by 1) is needed for GCC to generate CMOV
            first += comp(first[half], value) * (length - half);
            length = half;
        }
        return first;
    }

}

