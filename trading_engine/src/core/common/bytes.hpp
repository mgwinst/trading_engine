#pragma once

#include <cstring>
#include <cstddef>
#include <cstdint>
#include <concepts>
#include <bit>

#include "macros.hpp"

[[nodiscard]] FORCE_INLINE uint8_t load_byte(const std::byte* __restrict__ p) noexcept
{
    return std::to_integer<uint8_t>(*p);
}

template <std::integral T>
[[nodiscard]] FORCE_INLINE T loadu(const std::byte* __restrict__ p) noexcept
{
    T value;
    std::memcpy(&value, p, sizeof(T));
    return value;
}

template <std::integral T>
[[nodiscard]] FORCE_INLINE T loadu_bswap(const std::byte* __restrict__ p) noexcept
{
    T value;
    std::memcpy(&value, p, sizeof(T));
    return std::byteswap(value);
}

template <typename T>
requires std::is_trivially_copyable_v<T>
[[nodiscard]] FORCE_INLINE bool loadu_at(std::span<const std::byte> s, std::size_t offset, T& output) noexcept
{
    if (offset + sizeof(T) > s.size()) [[unlikely]] 
        return false;

    std::memcpy(&output, s.data() + offset, sizeof(T));
    return true;
}

template <typename T>
requires std::is_trivially_copyable_v<T>
[[nodiscard]] FORCE_INLINE std::optional<T> loadu_at(std::span<const std::byte> s, std::size_t offset) noexcept
{
    if (offset + sizeof(T) > s.size()) [[unlikely]] 
        return std::nullopt;

    T value;
    std::memcpy(&value, s.data() + offset, sizeof(T));
    return value;
}

// this doesn't belong in this header...
template <typename T>
inline constexpr bool ptr_in_range(const T* ptr, const T* begin, const T* end)
{
    return ptr >= begin && ptr <= end;
}