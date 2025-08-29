clang++ -std=c++23 test_main.cpp ../core/orderbook/OrderBook.cpp \
    -I../ -I../core/ \
    -lgtest -lgtest_main && ./a.out
