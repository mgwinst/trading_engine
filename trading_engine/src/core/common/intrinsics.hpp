#pragma once

inline void prefetch(const void* p) noexcept
{
    __builtin_prefetch(p, 0, 3);
}