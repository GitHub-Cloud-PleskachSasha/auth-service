cmake_minimum_required(VERSION 3.22)

project(StockExchange)

include_directories(${CMAKE_SOURCE_DIR}/lib/cpp-httplib)
include_directories(${CMAKE_SOURCE_DIR}/lib/mysql_orm/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

configure_file(${CMAKE_SOURCE_DIR}/src/key.pem ${CMAKE_BINARY_DIR}/key.pem COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/cert.pem ${CMAKE_BINARY_DIR}/cert.pem COPYONLY)

find_package(OpenSSL REQUIRED)

set(GEN_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GEN_DIR})

find_program(ODB_COMPILER odb)
if(NOT ODB_COMPILER)
  message(FATAL_ERROR "Не знайдено ODB компілятор!")
endif()

set(PERSON_HXX ${CMAKE_SOURCE_DIR}/src/person.hxx)
set(PERSON_ODB_CPP ${GEN_DIR}/person-odb.cxx)

add_custom_command(
  OUTPUT ${GEN_DIR}/person-odb.cxx ${GEN_DIR}/person-odb.hxx
  COMMAND odb -d mysql --generate-query --generate-schema --output-dir ${GEN_DIR} ${CMAKE_SOURCE_DIR}/src/person.hxx
  DEPENDS ${CMAKE_SOURCE_DIR}/src/person.hxx
)

include_directories(${GEN_DIR})
add_executable(StockExchange src/StockExchange.cpp ${GEN_DIR}/person-odb.hxx ${GEN_DIR}/person-odb.cxx) 

target_compile_features(StockExchange PUBLIC cxx_std_17)

target_link_libraries(StockExchange 
  OpenSSL::SSL 
  OpenSSL::Crypto
  odb
  odb-mysql
)	

find_library(MYSQL_CLIENT_LIBRARY mysqlclient)
if(NOT MYSQL_CLIENT_LIBRARY)
  message(FATAL_ERROR "mysqlclient not found")
endif()
target_link_libraries(StockExchange ${MYSQL_CLIENT_LIBRARY})

add_custom_command(
    TARGET StockExchange POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/src/key.pem $<TARGET_FILE_DIR:StockExchange>/key.pem
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/src/cert.pem $<TARGET_FILE_DIR:StockExchange>/cert.pem
)
