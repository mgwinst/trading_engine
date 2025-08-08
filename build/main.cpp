#include "../core/sys/memory.hpp"
#include "../core/common/macros.hpp"
#include "../core/common/allocators.hpp"

int main() 
{
    std::vector<std::byte, PageAlignedAllocator<std::byte>> buffer;
    buffer.reserve(1024);

    macros::ASSERT(pin_buffer(buffer) != -1, "pin_buffer() failed", macros::SOURCE_LOCATION(), errno);
    macros::ASSERT(unpin_buffer(buffer) != -1, "unpin_buffer() failed", macros::SOURCE_LOCATION(), errno);

    return 0;
}

