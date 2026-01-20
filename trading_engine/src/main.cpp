#include <print>

#include "core/common/queues/SPSCQueue.hpp"
#include "core/network/message.hpp"

int main()
{
    
    // uint8_t network_buffer[] = {'R', 0x15, 0x76, 0, 0, 0, 0, 0, 0, 0, 0, 'N', 'V', 'D', 'A', 0x20, 0x20, 0x20, 0x20};

    SPSCQueue<int> queue{ 1024 };
    
    std::array<float, 8> nums{1, 3, 888, 42, 17, 91, 92, 10};

    for (const auto& x : nums)
        queue.try_push(x);

    while (queue.size())
        std::print("{} ", queue.try_pop());

    std::println();
}