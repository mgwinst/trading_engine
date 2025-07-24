#include "common/macros.h"

int main() {
    int a = 10;
    int b = 20;
    common::ASSERT(a == b, "a does not equal b!");

    std::print("never prints\n");
}