cmake_minimum_required(VERSION 3.22)

project(AuthService)

include_directories(${CMAKE_SOURCE_DIR}/lib/cpp-httplib)
include_directories(${CMAKE_SOURCE_DIR}/lib/mysql_orm/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/jwt-cpp/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/nlohmann/json/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/googletest)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/models)
include_directories(${CMAKE_SOURCE_DIR}/src/services)
include_directories(${CMAKE_SOURCE_DIR}/src/repositories)

configure_file(${CMAKE_SOURCE_DIR}/src/key.pem ${CMAKE_BINARY_DIR}/key.pem COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/cert.pem ${CMAKE_BINARY_DIR}/cert.pem COPYONLY)

find_package(OpenSSL REQUIRED)

set(GEN_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GEN_DIR})

find_program(ODB_COMPILER odb)
if(NOT ODB_COMPILER) 
  message(FATAL_ERROR "ODB compiler not found!")
endif()

add_custom_command(
  OUTPUT ${GEN_DIR}/person-odb.cxx ${GEN_DIR}/person-odb.hxx
  COMMAND odb -d mysql --generate-query --generate-schema --output-dir ${GEN_DIR} ${CMAKE_SOURCE_DIR}/src/models/person.hxx
  DEPENDS ${CMAKE_SOURCE_DIR}/src/models/person.hxx
)

include_directories(${GEN_DIR})
add_executable(AuthService src/main.cpp ${GEN_DIR}/person-odb.hxx ${GEN_DIR}/person-odb.cxx) 

target_compile_features(AuthService PUBLIC cxx_std_17)

target_link_libraries(AuthService 
  OpenSSL::SSL 
  OpenSSL::Crypto
  odb
  odb-mysql
)	

add_custom_command(
    TARGET AuthService POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/src/key.pem $<TARGET_FILE_DIR:AuthService>/key.pem
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/src/cert.pem $<TARGET_FILE_DIR:AuthService>/cert.pem
)

# --------------------- Testing ---------------------

enable_testing()

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/googletest)

include_directories(${CMAKE_SOURCE_DIR}/lib/googletest/googletest/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/googletest/googlemock/include)

add_executable(AuthServiceTests tests/gTest.cpp)

target_link_libraries(AuthServiceTests 
    gtest 
    gtest_main 
    gmock 
    gmock_main 
    pthread
    OpenSSL::SSL 
    OpenSSL::Crypto
)

add_test(NAME AuthServiceTests COMMAND AuthServiceTests)
