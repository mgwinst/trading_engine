#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>

#include "SPSCQueue.hpp"

#define SIZE (4 * 1024 * 1024) // 4MB

// SymbolDirectory::instance()::index() -> index valid? -> uint8_t index used for map of queues

template <typename T>
class SPSCQueuePool
{
public:
    using Queue = SPSCQueue<T>;
    using QueueHandle = std::shared_ptr<Queue>;

    SPSCQueuePool(const SPSCQueuePool&) = delete;
    SPSCQueuePool& operator=(const SPSCQueuePool&) = delete;
    SPSCQueuePool(SPSCQueuePool&&) = delete;
    SPSCQueuePool& operator=(SPSCQueuePool&&) = delete;

    // template the key type?
    QueueHandle& operator[](const uint8_t index) noexcept
    {
        return queues_[index];
    }

    static SPSCQueuePool& instance() noexcept
    {
        static SPSCQueuePool message_queues;
        return message_queues;
    }

    void add_queues(const uint8_t index) noexcept
    {
        std::scoped_lock lock{ mtx_ };

        auto q = std::make_shared<Queue>(SIZE);
        queues_.insert({index, q});
    }

private:
    SPSCQueuePool() {}

    mutable std::mutex mtx_{ };
    std::unordered_map<uint8_t, QueueHandle> queues_;
};