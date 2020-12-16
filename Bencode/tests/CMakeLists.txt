cmake_minimum_required(VERSION 3.10.2)

project("Bencode Unit Tests" VERSION 0.0.0 DESCRIPTION "Bencode C++ Library Catch Unit Tests")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

set(TEST_EXECUTABLE ${BENCODE_LIBRARY_NAME}_tests)

set(TEST_SOURCES
    Bencode_tests.cpp
    catch.cpp
)

add_executable(${TEST_EXECUTABLE} ${TEST_SOURCES})
target_include_directories(${TEST_EXECUTABLE} PUBLIC ../include)

add_test(NAME ${TEST_EXECUTABLE} COMMAND ${TEST_EXECUTABLE})

target_link_libraries(${TEST_EXECUTABLE})
