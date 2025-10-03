#!/bin/bash

# clang++ -std=c++23 -g -Werror -Wall -Wextra -Wconversion -I../ main.cpp && ./a.out 2> log/error.txt

TRADING_ENGINE_HOME=/home/matteo/trading_engine

clang++ -std=c++23 -g -I$TRADING_ENGINE_HOME -I$TRADING_ENGINE_HOME/core -I$TRADING_ENGINE_HOME/core/common -I$TRADING_ENGINE_HOME/deps main.cpp \
    $TRADING_ENGINE_HOME/core/orderbook/L2/orderbook.cpp $TRADING_ENGINE_HOME/core/network/socket/raw_socket.cpp \
    $TRADING_ENGINE_HOME/core/network/socket/feed_handler.cpp $TRADING_ENGINE_HOME/core/itch/msg_handler.cpp \
    $TRADING_ENGINE_HOME/core/itch/msg_parser.cpp && sudo ./a.out


