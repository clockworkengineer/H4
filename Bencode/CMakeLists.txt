cmake_minimum_required(VERSION 3.17.0)

project("Bencode C++ library" VERSION 0.0.0 DESCRIPTION "Bencode C++ Static Library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

set(BENCODE_LIBRARY_NAME Bencode)

# Enable unit tests

enable_testing()

# Bencode sources and includes

set (BENCODE_SOURCES
    ./classes/Bencode.cpp
)

set (BENCODE_INCLUDES
    ./include/Bencode.hpp
)

# Bencode library

add_library(${BENCODE_LIBRARY_NAME} STATIC ${BENCODE_SOURCES})
target_include_directories(${BENCODE_LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${BENCODE_LIBRARY_NAME})

add_subdirectory(tests)


