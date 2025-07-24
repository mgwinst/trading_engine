#pragma once

#include <print>

namespace common {

    inline auto ASSERT(bool cond, const std::string_view msg) noexcept {
        if (!cond) [[unlikely]] {
            std::print("{}\n", msg);
            exit(EXIT_FAILURE);
        }
    }

}