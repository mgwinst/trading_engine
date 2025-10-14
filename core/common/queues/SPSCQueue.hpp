#pragma once

#include <memory>
#include <utility>
#include <optional>
#include <type_traits>
#include <atomic>
#include <span>
#include <limits>
#include <new>

#include "common/concepts.hpp"

#ifdef __cpp_lib_hardware_interference_size
    inline static constexpr std::size_t cache_line_size {std::hardware_destructive_interference_size};
#else
    inline static constexpr std::size_t cache_line_size {64};
#endif

template <typename T, std::size_t Capacity, typename Alloc = std::allocator<T>>
requires is_power_of_two<Capacity>
class SPSCQueue : private Alloc 
{
public:
    explicit SPSCQueue(const Alloc& alloc = Alloc{}) : Alloc{ alloc }
    {
        ring_ = std::allocator_traits<Alloc>::allocate(*this, Capacity);
    }

    ~SPSCQueue() 
    {
        while (!empty()) {
            ring_[get_index(read_pointer_)].~T();
            read_pointer_++;
        }

        std::allocator_traits<Alloc>::deallocate(*this, ring_, Capacity);
    }

    auto try_push(const T& value)
    {
        std::size_t write_pointer = write_pointer_.load(std::memory_order_relaxed);
        if (full(write_pointer, cached_read_pointer_)) {
            cached_read_pointer_ = read_pointer_.load(std::memory_order::acquire);
            if (full(write_pointer, cached_read_pointer_)) {
                return false;
            }
        }

        new (element(write_pointer)) T(value);
        write_pointer_.store(write_pointer_ + 1, std::memory_order_release);

        return true;
    }

    auto try_push(T&& value)
    {
        std::size_t write_pointer = write_pointer_.load(std::memory_order_relaxed);
        if (full(write_pointer, cached_read_pointer_)) {
            cached_read_pointer_ = read_pointer_.load(std::memory_order::acquire);
            if (full(write_pointer, cached_read_pointer_)) {
                return false;
            }
        }

        new (element(write_pointer)) T(std::move(value));
        write_pointer_.store(write_pointer_ + 1, std::memory_order_release);

        return true;
    }

    T try_pop()
    {
        auto read_pointer = read_pointer_.load(std::memory_order_relaxed);
        if (empty(cached_write_pointer_, read_pointer)) {
            cached_write_pointer_ = write_pointer_.load(std::memory_order::acquire);
            if (empty(cached_write_pointer_, read_pointer)) {
                return {};
            }
        }
        
        auto value = *element(read_pointer);
        element(read_pointer)->~T();
        read_pointer_.store(read_pointer + 1, std::memory_order_release);

        return value;
    }   

    bool try_pop(T& output)
    {
        auto read_pointer = read_pointer_.load(std::memory_order_relaxed);
        if (empty(cached_write_pointer_, read_pointer)) {
            cached_write_pointer_ = write_pointer_.load(std::memory_order::acquire);
            if (empty(cached_write_pointer_, read_pointer)) {
                return false;
            }
        }
        
        output = std::move(*element(read_pointer));
        element(read_pointer)->~T();
        read_pointer_.store(read_pointer + 1, std::memory_order_release);

        return true;
    }

    constexpr auto capacity() const { return Capacity; }
    auto size() const { return write_pointer_ - read_pointer_; }
    auto empty() const { return size() == 0; }
    auto full() const { return size() == capacity(); }

private:
    T* ring_{ nullptr };

    alignas(cache_line_size) std::atomic<std::size_t> write_pointer_{ 0 };
    alignas(cache_line_size) std::size_t cached_write_pointer_{ 0 };
    alignas(cache_line_size) std::atomic<std::size_t> read_pointer_{ 0 };
    alignas(cache_line_size) std::size_t cached_read_pointer_{ 0 };

    static_assert(std::atomic<std::size_t>::is_always_lock_free);

    auto empty(std::size_t write_pointer, std::size_t read_pointer) const { return write_pointer - read_pointer == 0; }
    auto full(std::size_t write_pointer, std::size_t read_pointer) const { return (write_pointer - read_pointer) == Capacity; }
    auto get_index(std::size_t pos) const { return (pos & (capacity() - 1)); }
    auto element(std::size_t pos) { return &ring_[get_index(pos)]; }
};

