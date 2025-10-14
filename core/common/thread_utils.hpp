#pragma once

#include <print>
#include <iostream>
#include <thread>
#include <atomic>
#include <unistd.h>
#include <chrono>
#include <functional>

namespace common {

    inline auto set_thread_core(int core_id) noexcept {
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(core_id, &cpu_set); // add core to set
        return (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_set) == 0);
    }
    
    template <typename F, typename... Args>
    inline std::optional<std::jthread> create_and_pin_thread(int core_id, F&& func, Args&&... args) noexcept {
        std::atomic_bool running, failed;
        
        auto thread_body = [&] {
            if (core_id >= 0 && !set_thread_core(core_id)) {
                std::print(std::cerr, "failed to set core affinity for {} to core {}\n", pthread_self(), core_id);
                failed = true;
                return;
            }

            std::print("set core affinity for {} to core {}\n", pthread_self(), core_id);
            running = true;
            std::invoke(std::forward<F>(func), std::forward<Args>(args)...);
        };

        auto t = std::jthread{ thread_body };
 
        using namespace std::chrono_literals;
        while (!running && !failed) {
            std::this_thread::sleep_for(100ms);
        }

        if (failed) {
            return std::nullopt;
        }

        return std::move(t);
    };

    
    




}