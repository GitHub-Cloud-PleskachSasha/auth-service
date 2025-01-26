cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(StockExchange)

include_directories(${CMAKE_SOURCE_DIR}/libs/cpp-httplib)

configure_file(${CMAKE_SOURCE_DIR}/src/key.pem ${CMAKE_BINARY_DIR}/key.pem COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/cert.pem ${CMAKE_BINARY_DIR}/cert.pem COPYONLY)
find_package(OpenSSL REQUIRED)

add_executable(StockExchange src/StockExchange.cpp)

target_link_libraries(StockExchange OpenSSL::SSL)	

#maybe delete
add_custom_command(
    TARGET StockExchange POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/src/key.pem $<TARGET_FILE_DIR:StockExchange>/key.pem
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/src/cert.pem $<TARGET_FILE_DIR:StockExchange>/cert.pem
)