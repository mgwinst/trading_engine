#!/bin/bash

# clang++ -std=c++23 -g -Werror -Wall -Wextra -Wconversion -I../ main.cpp && ./a.out 2> log/error.txt

clang++ -std=c++23 -g -Werror -Wall -I../ -I../../core/ -I../../core/common \
    main.cpp ../../core/network/socket/raw_socket.cpp ../../core/network/socket/feed_handler.cpp && sudo ./a.out
