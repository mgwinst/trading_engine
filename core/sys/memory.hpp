#pragma once

#include <sys/mman.h>
#include <cstdlib>
#include <vector>
#include <concepts>

template <typename T>
concept BufferType = requires(T t) 
{
    { t.data() } -> std::convertible_to<const void*>;
    { t.capacity() } -> std::integral;
};

auto pin_buffer(const BufferType auto& buffer) -> int32_t
{
    using elem_type = std::remove_pointer_t<decltype(buffer.data())>;
    if (mlock(buffer.data(), buffer.capacity() * sizeof(elem_type)) == -1)
        return -1;
    return 0;
}

auto unpin_buffer(const BufferType auto& buffer) -> int32_t
{
    using elem_type = std::remove_pointer_t<decltype(buffer.data())>;
    if (munlock(buffer.data(), buffer.capacity() * sizeof(elem_type)) == -1)
        return -1;
    return 0;
}
