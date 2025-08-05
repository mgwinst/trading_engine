#pragma once

#include <source_location>
#include <cstring>
#include <string_view>
#include <iostream>
#include <print>
#include <chrono>

namespace macros 
{   
    inline auto SOURCE_LOCATION(std::source_location location = std::source_location::current()) noexcept
    {
        return std::format("{}:{}:{}: {}", location.file_name(), location.line(), location.column(), location.function_name());
    }

    inline auto ASSERT(bool cond, const std::string_view message) noexcept 
    {
        if (!cond) [[unlikely]] {
            std::println(std::cerr, "{} | {} | error: {} [{}]", macros::SOURCE_LOCATION(), message, std::strerror(errno), errno);
            exit(EXIT_FAILURE);
        }
    }

    // "message" -> function that failed
    // internally handles errno
    void LOG_ERROR(std::string_view message) noexcept
    {
        std::println(std::cerr, "{} | {} | error: {} [{}]", macros::SOURCE_LOCATION(), message, std::strerror(errno), errno);
    }

    template <typename T>
    inline auto CURRENT_TIME(std::string& time_str) noexcept -> std::string&
    {
        auto time_in_nanos = std::chrono::duration_cast<T>(std::chrono::high_resolution_clock()::now().time_since_epoch()).count();
        time_str = std::to_string(time_in_nanos);
        return time_str;
    }
    
}