#!/bin/bash

# clang++ -std=c++23 -g -Wall -Wextra -I../ main.cpp && ./a.out 2> log/error.txt
clang++ -std=c++23 -g -I../ main.cpp && ./a.out
