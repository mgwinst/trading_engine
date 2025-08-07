#include <sys/socket.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <array>
#include <vector>
#include <cstddef>
#include <concepts>

template <typename T>
concept BufferType = requires(T t) {
    { t.data() } -> std::convertible_to<const void*>;
    { t.capacity() } -> std::integral;
};

auto pin_buffer(const BufferType auto& buffer) -> void
{
    using elem_type = std::remove_pointer_t<decltype(buffer.data())>;
    mlock(buffer.data(), buffer.capacity() * sizeof(elem_type));
}

auto unpin_buffer(const BufferType auto& buffer) -> void
{
    using elem_type = std::remove_pointer_t<decltype(buffer.data())>;
    munlock(buffer.data(), buffer.capacity() * sizeof(elem_type));
}

int main() 
{
    std::vector<std::byte> buffer;
    buffer.reserve(1024);

    pin_buffer(buffer);
    unpin_buffer(buffer);

    return 0;
}
