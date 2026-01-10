#pragma once

#include <type_traits>
#include <bit>

template <typename T>
inline constexpr bool always_false_v = false;

template <typename T>
constexpr bool is_byte = (sizeof(T) == 1) && std::is_unsigned_v<T>;

template <typename T>
concept ByteType = is_byte<T> || std::is_same_v<T, std::byte>;

template <std::size_t S>
concept is_power_of_two = std::has_single_bit(S);

