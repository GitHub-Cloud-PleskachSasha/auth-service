cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(StockExchange)

include_directories(${CMAKE_SOURCE_DIR}/libs/cpp-httplib)

find_package(OpenSSL REQUIRED)

add_executable(StockExchange src/StockExchange.cpp)

target_link_libraries(StockExchange OpenSSL::SSL)