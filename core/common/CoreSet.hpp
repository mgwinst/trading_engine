#pragma once

#include <atomic>
#include <bitset>
#include <array>
#include <ranges>
#include <algorithm>

#define TOTAL_CORES 16

class CoreSet
{
public:
    static_assert(std::atomic<std::bitset<TOTAL_CORES>>::is_always_lock_free, "std::atomic<std::bitset<N>> must be lock-free");

    static CoreSet& instance() noexcept
    {
        static CoreSet cores{};
        return cores;
    }

    CoreSet(const CoreSet&) = delete;
    CoreSet& operator=(const CoreSet&) = delete;
    CoreSet(CoreSet&&) = delete;
    CoreSet& operator=(CoreSet&&) = delete;

    int claim_core()
    {
        return find_available_core();
    }

    void release_core(int core_id)
    {
        release_available_core(core_id);
    }

    std::bitset<TOTAL_CORES> current_core_set()
    {
        auto cores = available_cores_.load(std::memory_order_acquire);
        return cores;
    }

    
private:
    std::atomic<std::bitset<TOTAL_CORES>> available_cores_;

    CoreSet() noexcept
    {
        auto all_available = std::bitset<TOTAL_CORES>{};
        all_available.set();
        available_cores_.store(all_available, std::memory_order_release);
    }

    int find_available_core()
    {
        while (true) {
            auto expected = available_cores_.load(std::memory_order_acquire);

            std::size_t idx = 0;
            while (idx < expected.size() && !expected.test(idx)) {
                idx++; 
            }

            if (idx == expected.size())
                return -1;

            auto desired = expected;
            desired.set(idx, 0);

            if (available_cores_.compare_exchange_strong(expected, desired, 
                                                         std::memory_order_release, 
                                                         std::memory_order_acquire)) {
                return static_cast<int>(idx);
            }
        }
    }

    void release_available_core(int core_id)
    {
        while (true) {
            auto expected = available_cores_.load(std::memory_order_acquire);

            if (expected.test(core_id)) {
                return;
            }

            auto desired = expected;
            desired.set(core_id, 1);

            if (available_cores_.compare_exchange_strong(expected, desired,
                                                         std::memory_order_release,
                                                         std::memory_order_acquire)) {
                return;
            }
        }
    }

};