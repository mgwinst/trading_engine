#include <print>
#include <algorithm>
#include <sys/socket.h>

#include "network/socket/raw_socket.hpp"
#include "common/queues/spsc_queue.hpp"

int main()
{
    // network::RawSocket socket{ "eno1" };

    SPSCQueue<std::byte> queue{1024};

    std::byte bytes[] {std::byte{0x04}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}};

    std::byte rx_buffer[1024];

    auto bytes_written = queue.write(bytes);
    auto bytes_read = queue.read(rx_buffer);

    int32_t* num = reinterpret_cast<int32_t*>(rx_buffer);

    std::print("{}\n", *num);
    std::print("bytes_written: {}\n", bytes_written);
    std::print("bytes_read: {}\n", bytes_read);
    std::print("queue write counter: {}\n", queue.write_counter_.load());
    std::print("queue read counter: {}\n", queue.read_counter_.load());
}
