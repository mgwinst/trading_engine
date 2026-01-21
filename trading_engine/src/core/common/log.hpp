#pragma once

#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "queues/SPSCQueue.hpp"

inline constexpr std::size_t MSG_CAP = 32;

struct Log
{
    char msg_[MSG_CAP];
    uint64_t a1_, a2_, a3_, a4_;
};

static_assert(sizeof(Log) == 64, "sizeof(Log) != 64 bytes");

struct Logger
{
    SPSCQueue<Log> log_queue_{};

    template <std::size_t N>
    void log(const char (&msg)[N], const uint64_t arg1 = 0, const uint64_t arg2 = 0, const uint64_t arg3 = 0, const uint64_t arg4 = 0)
    {
        static_assert(N <= MSG_CAP, "string literal must be <= 32 bytes");

        Log record{};

        std::memcpy(record.msg_, msg, N);

        record.a1_ = arg1;
        record.a2_ = arg2;
        record.a3_ = arg3;
        record.a4_ = arg4;

        log_queue_.try_push(record);
    }
};