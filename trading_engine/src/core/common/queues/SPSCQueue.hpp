#pragma once

#include <bit>
#include <concepts>
#include <memory>
#include <utility>
#include <atomic>
#include <new>

#include "../macros.hpp"

#ifdef __cpp_lib_hardware_interference_size
    inline static constexpr std::size_t cache_line_size{ std::hardware_destructive_interference_size };
#else
    inline static constexpr std::size_t cache_line_size{ 64 };
#endif

constexpr inline std::size_t DEFAULT_SPSC_QUEUE_SIZE{ 4 * 1024 * 1024 };

template <typename T>
concept is_lock_free_atomic = requires { std::atomic<T>::is_always_lock_free; };

template <typename T>
struct alignas(cache_line_size) Slot
{
    T value;
};

template <typename T, typename Alloc = std::allocator<Slot<T>>>
requires is_lock_free_atomic<std::size_t>
class SPSCQueue : private Alloc 
{
public:
    SPSCQueue(const Alloc& alloc = Alloc{}) : Alloc{ alloc }
    {
        ring_ = std::allocator_traits<Alloc>::allocate(*this, DEFAULT_SPSC_QUEUE_SIZE);
        capacity_ = DEFAULT_SPSC_QUEUE_SIZE;
    }

    explicit SPSCQueue(const std::size_t capacity, const Alloc& alloc = Alloc{}) : Alloc{ alloc }
    {
        if (!std::has_single_bit(capacity))
            error_exit("invalid buffer size");

        ring_ = std::allocator_traits<Alloc>::allocate(*this, capacity);
        capacity_ = capacity;
    }

    ~SPSCQueue() 
    {
        while (!empty()) {
            ring_[get_index(read_pointer_)].value.~T();
            read_pointer_++;
        }

        std::allocator_traits<Alloc>::deallocate(*this, ring_, capacity_);
    }

    template <typename U = T>
        requires std::constructible_from<T, std::remove_cvref_t<U>>
    bool try_push(U&& value)
    {
        std::size_t write_pointer = write_pointer_.load(std::memory_order_relaxed);
        if (full(write_pointer, cached_read_pointer_)) {
            cached_read_pointer_ = read_pointer_.load(std::memory_order::acquire);
            if (full(write_pointer, cached_read_pointer_)) {
                return false;
            }
        }

        ::new (element(write_pointer)) T{ std::forward<T>(value) };
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

    constexpr auto capacity() const { return capacity_; }
    auto size() const { return write_pointer_ - read_pointer_; }
    auto empty() const { return size() == 0; }
    auto full() const { return size() == capacity(); }

private:
    Slot<T>* ring_{ nullptr };
    std::size_t capacity_;

    static_assert(sizeof(Slot<T>) >= cache_line_size, "Slot<T> must occupy at least one cache line");

    alignas(cache_line_size) std::atomic<std::size_t> write_pointer_{ 0 };
    alignas(cache_line_size) std::size_t cached_write_pointer_{ 0 };
    alignas(cache_line_size) std::atomic<std::size_t> read_pointer_{ 0 };
    alignas(cache_line_size) std::size_t cached_read_pointer_{ 0 };

    auto empty(std::size_t write_pointer, std::size_t read_pointer) const { return write_pointer - read_pointer == 0; }
    auto full(std::size_t write_pointer, std::size_t read_pointer) const { return (write_pointer - read_pointer) == capacity(); }
    auto get_index(std::size_t pos) const { return (pos & (capacity() - 1)); }
    auto element(std::size_t pos) { return &ring_[get_index(pos)].value; }
};

