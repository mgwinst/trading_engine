#pragma once

#include <atomic>
#include <span>
#include <new>

#ifdef __cpp_lib_hardware_interference_size
    inline static constexpr std::size_t cache_line_size = std::hardware_destructive_interference_size;
#else
    inline static constexpr std::size_t cache_line_size{64};
#endif

namespace common {

    struct SPMCQueue {
        alignas(cache_line_size) std::atomic_uint64_t _read_counter{0};
        alignas(cache_line_size) std::atomic_uint64_t _write_counter{0};
        alignas(cache_line_size) uint8_t buffer[0];
    };

    struct SPMCQueueProducer {
        void write(std::span<std::byte> buffer);
    };

    struct SPMCQueueConsumer {
        uint64_t try_read(std::span<std::byte> buffer);
    };

}


