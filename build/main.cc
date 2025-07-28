#include "common/macros.h"
#include "network/tcp_socket.h"

int main() {
    auto ip = network::get_iface_ip("lo");
    std::print("{}\n", ip);

    return 0;
}
