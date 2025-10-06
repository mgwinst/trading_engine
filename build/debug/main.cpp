#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <print>
#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include <array>

/*
#include "common/queues/MessageQueues.hpp"
#include "common/thread_utils.hpp"
#include "common/json_parser.hpp"
#include "network/socket/raw_socket.hpp"
#include "network/socket/feed_handler.hpp"
#include "orderbook/orderbook_manager.hpp"
#include "parser/msg_types.hpp"
*/

#include "common/CoreSet.hpp"


int main()
{
    auto& cores = CoreSet::instance();
    
    auto core_id0 = cores.claim_core();
    auto core_id1 = cores.claim_core();
    auto core_id2 = cores.claim_core();

    std::cout << "coreid0 " << core_id0 << std::endl;
    std::cout << "coreid1 " << core_id1 << std::endl;
    std::cout << "coreid2 " << core_id2 << std::endl;

    auto available_cores = cores.current_core_set();

    std::print("available cores: ");
    for (int i = 0; i < available_cores.size(); i++) {
        std::cout << available_cores[i];
    }
    std::println();

    cores.release_core(core_id1);   

    available_cores = cores.current_core_set();

    std::print("available cores: ");
    for (int i = 0; i < available_cores.size(); i++) {
        std::cout << available_cores[i];
    }
    std::println();

}   
