#pragma once

#include <print>
#include <source_location>
#include <string_view>

namespace macros 
{   
    inline auto SOURCE_LOCATION(std::source_location location = std::source_location::current()) noexcept
    {
        return std::format("{}:{}:{}: {}", location.file_name(), location.line(), location.column(), location.function_name());
    }

    inline auto ASSERT(bool cond, const std::string_view message) noexcept 
    {
        if (!cond) [[unlikely]] {
            std::println("{}", message);
            exit(EXIT_FAILURE);
        }
    }
}