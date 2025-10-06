#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>

#include "common/queues/SPSCQueue.hpp"

#define SIZE (4 * 1024 * 1024)

template <typename T>
class MessageQueues
{
public:
    using Queue = SPSCQueue<T, SIZE>;
    using QueueHandle = std::shared_ptr<Queue>;

    MessageQueues(const MessageQueues&) = delete;
    MessageQueues& operator=(const MessageQueues&) = delete;
    MessageQueues(MessageQueues&&) = delete;
    MessageQueues& operator=(MessageQueues&&) = delete;

    QueueHandle& operator[](const std::string& key) noexcept
    {
        return queues_.at(key);
    }

    static MessageQueues& get_instance() noexcept
    {
        static MessageQueues message_queues;
        return message_queues;
    }

    void add_queues(const std::vector<std::string>& tickers) noexcept
    {
        std::scoped_lock lock{mtx_};

        for (auto ticker : tickers) {
            auto q = std::make_shared<Queue>();
            queues_.insert({ticker, q});
        }
    }

    void add_queues(std::string ticker) noexcept
    {
        std::scoped_lock lock{mtx_};

        auto q = std::make_shared<Queue>();
        queues_.insert({ticker, q});
    }

private:
    MessageQueues() {}

    mutable std::mutex mtx_{};
    std::unordered_map<std::string, QueueHandle> queues_;
};