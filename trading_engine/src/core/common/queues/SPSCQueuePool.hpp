#pragma once

#include <memory>
#include <unordered_map>
#include <mutex>

#include "SPSCQueue.hpp"

inline constexpr std::size_t SIZE{ 4 * 1024 * 1024 };

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

    QueueHandle& operator[](const uint64_t index) noexcept
    {
        return queues_[index];
    }

    static SPSCQueuePool& instance() noexcept
    {
        static SPSCQueuePool message_queues;
        return message_queues;
    }

    void add_queue(const uint64_t index) noexcept
    {
        std::scoped_lock lock{ mtx_ };

        auto q = std::make_shared<Queue>(SIZE);

        queues_.insert({index, q});
    }

private:
    SPSCQueuePool() {}

    mutable std::mutex mtx_{ };
    std::unordered_map<uint64_t, QueueHandle> queues_; // use faster hash table?
};