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

    inline auto ASSERT(bool cond, std::string_view message, std::string_view location) noexcept 
    {
        if (!cond) [[unlikely]] {
            std::println(std::cerr, "{} | error: {}", location, message);
            exit(EXIT_FAILURE);
        }
    }

    inline auto ASSERT(bool cond, std::string_view message, std::string_view location, int error) noexcept 
    {
        if (!cond) [[unlikely]] {
            std::println(std::cerr, "{} | {} | error: {} [{}]", location, message, std::strerror(error), error);
            exit(EXIT_FAILURE);
        }
    }

    inline void LOG_ERROR(std::string_view message, std::string_view location) noexcept
    {
        std::println(std::cerr, "{} | error: {}", location, message);
    }

    inline void LOG_ERROR(std::string_view message, std::string_view location, int error) noexcept
    {
        std::println(std::cerr, "{} | {} | error: {} [{}]", location, message, std::strerror(error), error);
    }

    #define exit_with_error(error) __exit_with_error(error, __FILE__, __func__, __LINE__)
}
