#pragma once

#include <cstring>
#include <memory>
#include <atomic>
#include <new>

#include "../concepts.hpp"

#define DEFAULT_BUFFER_SIZE (1<<22) // 4MB

#ifdef __cpp_lib_hardware_interference_size
    inline static constexpr std::size_t cache_line_size{ std::hardware_destructive_interference_size };
#else
    inline static constexpr std::size_t cache_line_size{ 64 };
#endif

template <ByteType T, std::size_t Capacity>
requires is_power_of_two<Capacity>
class CircularBuffer
{
public:
    CircularBuffer() : ring_{ std::make_unique<T[]>(Capacity) } {}

    CircularBuffer(const CircularBuffer&) = delete;
    CircularBuffer& operator=(const CircularBuffer&) = delete;
    CircularBuffer(CircularBuffer&&) = delete;
    CircularBuffer& operator=(CircularBuffer&&) = delete;

    std::size_t write(T* data, const std::size_t bytes_to_write)
    {
        const std::size_t write_counter = write_counter_.load(std::memory_order_relaxed);
        const std::size_t read_counter = read_counter_.load(std::memory_order_acquire);

        if (full(write_counter, read_counter) || bytes_to_write > space(write_counter, read_counter))
            return 0;

        const auto index = get_index(write_counter);
        const auto bytes_to_end = capacity() - index;

        if (bytes_to_write <= bytes_to_end) {
            std::memcpy(ring_.get() + index, data, bytes_to_write);
        } else {
            std::memcpy(ring_.get() + index, data, bytes_to_end);
            std::memcpy(ring_.get(), data + bytes_to_end, bytes_to_write - bytes_to_end);
        }

        write_counter_.store(write_counter + bytes_to_write, std::memory_order_release);

        return bytes_to_write;
    }
    
    std::size_t read(T* buffer, std::size_t bytes_to_read)
    {
        const std::size_t write_counter = write_counter_.load(std::memory_order_relaxed);
        const std::size_t read_counter = read_counter_.load(std::memory_order_acquire);

        if (empty(write_counter, read_counter))
            return 0;

        const auto bytes_in_queue = size(write_counter, read_counter);

        bytes_to_read = std::min(bytes_to_read, bytes_in_queue);

        const auto index = get_index(read_counter);
        const auto bytes_to_end = capacity() - index;

        if (bytes_to_read < bytes_to_end) {
            std::memcpy(buffer, ring_.get() + index, bytes_to_read);
        } else {
            std::memcpy(buffer, ring_.get() + index, bytes_to_end);
            std::memcpy(buffer + bytes_to_end, ring_.get(), bytes_to_read - bytes_to_end);
        }

        read_counter_.store(read_counter + bytes_to_read, std::memory_order_release);

        return bytes_to_read;
    }

    T* get_read_pointer()
    {
        std::size_t read_counter = read_counter_.load(std::memory_order_acquire);
        return &(ring_.get() + get_index(read_counter));
    }

private:
    std::unique_ptr<T[]> ring_{ nullptr };

    alignas(cache_line_size) std::atomic<std::size_t> read_counter_{ 0 };
    alignas(cache_line_size) std::atomic<std::size_t> write_counter_{ 0 };    
    // alignas(cache_line_size) std::atomic<std::size_t> cached_write_counter_{ 0 };
    // alignas(cache_line_size) std::atomic<std::size_t> cached_read_counter_{ 0 };

    static_assert(std::atomic<std::size_t>::is_always_lock_free);

    constexpr auto capacity() const { return Capacity; }
    auto size(std::size_t write_counter, std::size_t read_counter) const { return write_counter - read_counter; }
    auto full(std::size_t write_counter, std::size_t read_counter) const { return size(write_counter, read_counter) == capacity(); }
    auto empty(std::size_t write_counter, std::size_t read_counter) const { return size(write_counter, read_counter) == 0; }
    auto get_index(std::size_t counter) const { return (counter & (capacity() - 1)); }
    auto space(std::size_t write_counter, std::size_t read_counter) const { return capacity() - size(write_counter, read_counter); }
};
