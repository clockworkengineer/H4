cmake_minimum_required(VERSION 3.17.0)

project("Bencode Unit Tests" VERSION 0.0.1 DESCRIPTION "Bencode C++ Library Catch Unit Tests")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

set(TEST_EXECUTABLE ${BENCODE_LIBRARY_NAME}_Unit_Tests)

set(TEST_SOURCES
    Bencode_tests.cpp
)

add_executable(${TEST_EXECUTABLE} ${TEST_SOURCES})
target_include_directories(${TEST_EXECUTABLE} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${TEST_EXECUTABLE} ${BENCODE_LIBRARY_NAME} Catch2::Catch2WithMain)

add_test(NAME ${TEST_EXECUTABLE} COMMAND ${TEST_EXECUTABLE})

add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/testData testData)


