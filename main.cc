#include "common/macros.h"
#include "common/thread_utils.h"

int main() {
    int value = 1;   

    auto f1 = []() { std::print("hello world\n"); };
    auto f2 = [](int x) { 
        x += 1;
        std::print("{}\n", x); 
    };

    std::thread* t = common::create_and_pin_thread(0, f2, value);
    t->join();
}