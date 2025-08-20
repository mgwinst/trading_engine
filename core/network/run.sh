#!/bin/bash

clang++ -std=c++23 af_xdp.cpp -I../ && sudo ./a.out
