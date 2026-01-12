#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>

#include "SPSCQueue.hpp"

#define SIZE (4 * 1024 * 1024) // 4MB

template <typename T>
class MessageQueuePool
{
public:
    using Queue = SPSCQueue<T>;
    using QueueHandle = std::shared_ptr<Queue>;

    MessageQueuePool(const MessageQueuePool&) = delete;
    MessageQueuePool& operator=(const MessageQueuePool&) = delete;
    MessageQueuePool(MessageQueuePool&&) = delete;
    MessageQueuePool& operator=(MessageQueuePool&&) = delete;

    QueueHandle& operator[](const std::string& key) noexcept
    {
        return queues_.at(key);
    }

    static MessageQueuePool& instance() noexcept
    {
        static MessageQueuePool message_queues;
        return message_queues;
    }

    void add_queues(const std::vector<std::string>& tickers) noexcept
    {
        std::scoped_lock lock{ mtx_ };

        for (auto ticker : tickers) {
            auto q = std::make_shared<Queue>(SIZE);
            queues_.insert({ticker, q});
        }
    }

    void add_queues(std::string ticker) noexcept
    {
        std::scoped_lock lock{ mtx_ };

        auto q = std::make_shared<Queue>(SIZE);
        queues_.insert({ticker, q});
    }

private:
    MessageQueuePool() {}

    mutable std::mutex mtx_{ };
    std::unordered_map<uint8_t, QueueHandle> queues_;
};