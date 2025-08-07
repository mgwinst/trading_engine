#include <thread>

#include "common/macros.hpp"
#include "network/socket.hpp"

using namespace network::utilities;

void test() {
    Socket<TCP> client_socket, server_socket;
    server_socket.connect("", "lo", 3001, Listening::YES, Blocking::NO);
    client_socket.connect("", "lo", 3001, Listening::NO, Blocking::NO);
}

int main() {

}

