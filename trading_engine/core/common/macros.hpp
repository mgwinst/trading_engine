#pragma once

#include <source_location>
#include <string_view>
#include <iostream>
#include <cstring>
#include <print>

#define ASSERT(cond, msg) \
    { \
        auto loc = std::source_location::current(); \
        auto loc_str = std::format("{}:{}:{}: {}", loc.file_name(), loc.line(), loc.column(), loc.function_name()); \
        if (!cond) [[unlikely]] { \
            std::println(std::cerr, "{} | {} | error: {} [{}]", loc_str, msg, std::strerror(errno), errno); \
            exit(EXIT_FAILURE); \
        } \
    }

#define error_exit(msg) \
    { \
        auto loc = std::source_location::current(); \
        auto loc_str = std::format("{}:{}:{}: {}", loc.file_name(), loc.line(), loc.column(), loc.function_name()); \
        std::println(std::cerr, "{} | {} | error: {} [{}]", loc_str, msg, std::strerror(errno), errno); \
        std::exit(EXIT_FAILURE); \
    }

#define FORCE_INLINE inline __attribute__((always_inline))