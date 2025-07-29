#include <iostream>

#include "common/macros.hpp"
#include "network/Socket.hpp"

int main() {
    
    auto conf = network::SocketConfigFactory::from_interface("eno1", 1000, network::TransportLayerProtocol::TCP, true);

    if (conf)
        std::print("{}\n", conf.value().to_string());
    else {
        switch(conf.error()) {
            case network::SocketConfigError::Interface_IP_Not_Found:
                std::print(std::cerr, "Interface IP not found.\n");
                break;
            case network::SocketConfigError::Invalid_Port:
                std::print(std::cerr, "Invalid port number.\n");
        }
    }
    return 0;
}
