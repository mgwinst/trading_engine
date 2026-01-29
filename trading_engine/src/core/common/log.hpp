#pragma once

#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "macros.hpp"

inline constexpr std::size_t MSG_CAP = 32;

struct Log
{
    char msg_[MSG_CAP];
    uint64_t a1_, a2_, a3_, a4_;
};

static_assert(sizeof(Log) == 64, "sizeof(Log) != 64 bytes");

inline constexpr std::size_t LOG_BUF_SIZE = 1 << 20;
inline constexpr std::size_t BUF_LEN_BYTES = sizeof(Log) * LOG_BUF_SIZE;

struct Logger
{
    int32_t fd_{ -1 };
    Log* buffer_{ nullptr };
    std::size_t idx_;
    
    Logger(const char* path)
    {
        fd_ = open(path, O_RDWR | O_CREAT, 0644);
        if (fd_ < 0)
            error_exit("open()");

        if (ftruncate(fd_, BUF_LEN_BYTES) != 0) {
            close(fd_);
            error_exit("ftruncate()");
        }
        
        void* map = mmap(nullptr, BUF_LEN_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd_, 0);

        if (map == MAP_FAILED) {
            close(fd_);
            error_exit("mmap()");
        }

        madvise(map, BUF_LEN_BYTES, MADV_WILLNEED);

        buffer_ = reinterpret_cast<Log *>(map);
    }

    template <std::size_t N>
    void log(const char (&msg)[N], const uint64_t arg1 = 0, const uint64_t arg2 = 0, const uint64_t arg3 = 0, const uint64_t arg4 = 0)
    {
        static_assert(N <= MSG_CAP, "string literal must be <= 32 bytes");

        if (idx_ >= LOG_BUF_SIZE) [[unlikely]] return;

        Log record{};

        std::memcpy(record.msg_, msg, N);

        record.a1_ = arg1;
        record.a2_ = arg2;
        record.a3_ = arg3;
        record.a4_ = arg4;

        buffer_[idx_++] = record;
    }
};