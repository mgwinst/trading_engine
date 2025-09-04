#!/bin/bash

# clang++ -std=c++23 -g -Werror -Wall -Wextra -Wconversion -I../ main.cpp && ./a.out 2> log/error.txt

clang++ -std=c++23 -g -Werror -Wall -Wextra -Wconversion -I../ -I../../core/ -I../../core/common \
    main.cpp ../../core/network/socket/raw_socket.cpp && sudo ./a.out
