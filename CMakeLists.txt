cmake_minimum_required(VERSION 3.8)
project(engine VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBBPF REQUIRED IMPORTED_TARGET libbpf>=1.0)
pkg_check_modules(LIBXDP REQUIRED IMPORTED_TARGET libxdp>=1.0)

# nlohmann/json.hpp —> header-only
# URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.hpp

add_library(engine_lib STATIC
    trading_engine/src/core/network/xdp/xdp_loader.cpp
    trading_engine/src/core/network/feed_handler.cpp
    trading_engine/src/core/network/socket.cpp
    trading_engine/src/core/orderbook/orderbook_manager.cpp
    trading_engine/src/core/orderbook/orderbook.cpp
)

target_include_directories(engine_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/trading_engine/src/core
)

target_link_libraries(engine_lib PUBLIC
    Boost::boost
    PkgConfig::LIBBPF
    PkgConfig::LIBXDP
)

add_executable(${PROJECT_NAME} trading_engine/src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE engine_lib)

target_compile_options(${PROJECT_NAME} PRIVATE
    -march=native -fcolor-diagnostics
    -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion
    -Wshadow -Wundef -Werror=return-type
)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:
        -O0 -g3
        -fno-omit-frame-pointer
        -fsanitize=address,undefined,leak
        -fsanitize-address-use-after-scope
        -fno-sanitize-recover=all
    >
    $<$<CONFIG:Release>:
        -O3
        -flto=thin
        -DNDEBUG
    >
)

target_link_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:
        -fsanitize=address,undefined,leak
        -fsanitize-address-use-after-scope
        -fno-sanitize-recover=all
    >
    $<$<CONFIG:Release>:
        -flto=thin
    >
)

option(ENABLE_TESTS "Build unit tests" ON)
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS engine_tests
    USES_TERMINAL
    COMMENT "Running all tests..."
)

option(ENABLE_BENCHMARKS "Build Google Benchmark suite" OFF)
if(ENABLE_BENCHMARKS)
    add_subdirectory(extra/benchmark)
endif()