cmake_minimum_required(VERSION 3.8)

find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(WARNING "GoogleTest not found -> skipping tests. Install GoogleTest.")
    return()
endif()

# ctest engine_tests
add_executable(engine_tests
    orderbook/orderbook_test.cpp
)

target_link_libraries(engine_tests PRIVATE
    engine_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(engine_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
