#include <sys/socket.h>
#include <algorithm>
#include <print>

#include "network/socket/raw_socket.hpp"
#include "network/socket/feed_handler.hpp"

int main()
{
    network::RawSocket socket{"eno1"};
    network::FeedHandler feed_handler{};
}
