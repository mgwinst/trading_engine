#pragma once

#include <cstring>
#include <cstddef>
#include <cstdint>
#include <concepts>
#include <bit>

#include "macros.hpp"

[[nodiscard]] FORCE_INLINE uint8_t load_byte(const std::byte* p) noexcept
{
    return std::to_integer<uint8_t>(*p);
}

template <std::integral T>
[[nodiscard]] FORCE_INLINE T load_be(const std::byte* p) noexcept
{
    T value;
    std::memcpy(&value, p, sizeof(T));
    return std::byteswap(value);
}

template <std::integral T>
[[nodiscard]] FORCE_INLINE T load_le(const std::byte* p) noexcept
{
    T value;
    std::memcpy(&value, p, sizeof(T));
    return value;
}
