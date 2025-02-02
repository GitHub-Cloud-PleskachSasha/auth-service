cmake_minimum_required(VERSION 3.22)

project(StockExchange)

include_directories(${CMAKE_SOURCE_DIR}/lib/cpp-httplib)
include_directories(${CMAKE_SOURCE_DIR}/lib/mysql_orm/include)

configure_file(${CMAKE_SOURCE_DIR}/src/key.pem ${CMAKE_BINARY_DIR}/key.pem COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/cert.csr ${CMAKE_BINARY_DIR}/cert.csr COPYONLY)

#find_package(OpenSSL REQUIRED)
#find_package(MySQL REQUIRED)

add_executable(StockExchange src/StockExchange.cpp)

#target_link_libraries(StockExchange OpenSSL::SSL)	
target_link_libraries(StockExchange)

#maybe delete
add_custom_command(
    TARGET StockExchange POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/src/key.pem $<TARGET_FILE_DIR:StockExchange>/key.pem
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/src/cert.csr $<TARGET_FILE_DIR:StockExchange>/cert.csr
)