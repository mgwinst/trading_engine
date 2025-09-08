#pragma once

#include <cstring>
#include <memory>
#include <utility>
#include <atomic>
#include <span>
#include <new>

#include "queues/concepts.hpp"

#ifdef __cpp_lib_hardware_interference_size
    inline static constexpr std::size_t cache_line_size = std::hardware_destructive_interference_size;
#else
    inline static constexpr std::size_t cache_line_size{64};
#endif

inline auto power_of_two(std::size_t n)
{
    return (n & (n - 1)) == 0;
}

template <ByteType T>
class SPSCQueue
{
public:
    SPSCQueue(std::size_t capacity) 
    {
        if (capacity <= 0 || !power_of_two(capacity))
            throw std::invalid_argument("queue capacity must be power of 2.");

        capacity_ = capacity;
        ring_ = new T[capacity];
    }

    ~SPSCQueue()
    {
        delete[] ring_;
    }

    SPSCQueue(const SPSCQueue&) = delete;
    SPSCQueue& operator=(const SPSCQueue&) = delete;
    SPSCQueue(SPSCQueue&&) = delete;
    SPSCQueue& operator=(SPSCQueue&&) = delete;

    auto write(T* data, std::size_t bytes_to_write) -> std::size_t
    {
        std::size_t write_counter = write_counter_.load(std::memory_order_relaxed);
        std::size_t read_counter = read_counter_.load(std::memory_order_acquire);

        if (full(write_counter, read_counter) || bytes_to_write > space(write_counter, read_counter))
            return 0;

        auto index = get_index(write_counter);
        auto bytes_to_end = capacity_ - index;

        if (bytes_to_write <= bytes_to_end) {
            std::memcpy(ring_ + index, data, bytes_to_write);
        } else {
            std::memcpy(ring_ + index, data, bytes_to_end);
            std::memcpy(ring_, data + bytes_to_end, bytes_to_write - bytes_to_end);
        }

        write_counter_.store(write_counter + bytes_to_write, std::memory_order_release);

        return bytes_to_write;
    }
    
    auto read(T* buffer, std::size_t bytes_to_read) -> std::size_t
    {
        std::size_t write_counter = write_counter_.load(std::memory_order_relaxed);
        std::size_t read_counter = read_counter_.load(std::memory_order_acquire);

        if (empty(write_counter, read_counter))
            return 0;

        auto bytes_in_queue = size(write_counter, read_counter);

        bytes_to_read = std::min(bytes_to_read, bytes_in_queue);

        auto index = get_index(read_counter);
        auto bytes_to_end = capacity_ - index;

        if (bytes_to_read < bytes_to_end) {
            std::memcpy(buffer, ring_ + index, bytes_to_read);
        } else {
            std::memcpy(buffer, ring_ + index, bytes_to_end);
            std::memcpy(buffer + bytes_to_end, ring_, bytes_to_read - bytes_to_end);
        }

        read_counter_.store(read_counter + bytes_to_read, std::memory_order_release);

        return bytes_to_read;
    }

private:
    T* ring_{ nullptr };
    std::size_t capacity_{ 0 };

    alignas(cache_line_size) std::atomic<std::size_t> read_counter_{ 0 };
    alignas(cache_line_size) std::atomic<std::size_t> write_counter_{ 0 };    
    // alignas(cache_line_size) std::atomic<std::size_t> cached_write_counter_{ 0 };
    // alignas(cache_line_size) std::atomic<std::size_t> cached_read_counter_{ 0 };
    static_assert(std::atomic<std::size_t>::is_always_lock_free);

    auto size(std::size_t write_counter, std::size_t read_counter) const -> std::size_t { return write_counter - read_counter; }
    auto full(std::size_t write_counter, std::size_t read_counter) const -> bool { return size(write_counter, read_counter) == capacity_; }
    auto empty(std::size_t write_counter, std::size_t read_counter) const -> bool { return size(write_counter, read_counter) == 0; }
    auto get_index(std::size_t counter) const -> std::size_t { return (counter & (capacity_ - 1)); }
    auto space(std::size_t write_counter, std::size_t read_counter) const -> std::size_t { return capacity_ - size(write_counter, read_counter); }
};
