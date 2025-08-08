#pragma once

#include <numeric>
#include <unistd.h>
#include <cstdlib>
#include <new>

template <typename T>
struct PageAlignedAllocator {
    using value_type = T;

    PageAlignedAllocator() noexcept = default;
    PageAlignedAllocator(const PageAlignedAllocator&) noexcept = default;
    PageAlignedAllocator& operator=(const PageAlignedAllocator&) noexcept = default;
    PageAlignedAllocator(PageAlignedAllocator&&) noexcept = default;
    PageAlignedAllocator& operator=(PageAlignedAllocator&&) noexcept = default;

    template <typename U>
    struct rebind 
    {
        using other = PageAlignedAllocator<U>;
    };

    auto allocate(std::size_t n) -> T* 
    {
        if (n < 0 || n > max_size()) throw std::bad_array_new_length{};
        std::size_t page_size = getpagesize();
        T* ptr = static_cast<T*>(std::aligned_alloc(page_size, n * sizeof(T)));
        if (!ptr) throw std::bad_alloc{};
        return ptr;
    }

    auto deallocate(T* ptr, std::size_t n) -> void
    {
        std::free(ptr);
    }

    [[nodiscard]] auto max_size() -> std::size_t { return std::numeric_limits<std::size_t>::max() / sizeof(T); }
};

template <typename T, typename U>
auto operator==(const PageAlignedAllocator<T>&, const PageAlignedAllocator<U>&) -> bool { return true; }

template <typename T, typename U>
auto operator!=(const PageAlignedAllocator<T>&, const PageAlignedAllocator<U>&) -> bool { return false; }
