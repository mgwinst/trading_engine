#include "common/macros.h"
#include "network/socket.h"

int main() {
    
    auto conf = network::SocketConfigFactory::create_from_interface("lo", 1000, network::TransportLayerProtocol::UDP, true);
    if (conf.has_value())
        std::print("{}\n", conf.value().to_string());
    else
        std::print("interface doesn't exist\n");

    return 0;
}
