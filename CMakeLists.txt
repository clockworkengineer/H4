cmake_minimum_required(VERSION 3.17.0)

project("H4" VERSION 0.0.1 DESCRIPTION "H4 C++ Static Library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

add_subdirectory(Bencode)
add_subdirectory(JSON)

configure_file(./Bencode/BencodeConfig.h.in BencodeConfig.hpp)
configure_file(./JSON/JSONConfig.h.in JSONConfig.hpp)

set(H4_LIBRARY_NAME H4)

# JSON sources and includes

set (H4_SOURCES
    ./Bencode/classes/Bencode.cpp
    ./JSON/classes/JSON.cpp
)

set (H4_INCLUDES
    ./Bencode/include/Bencode.hpp
    ./Bencode/include/BencodeSources.hpp
    ./Bencode/include/BencodeDestinations.hpp
    ./JSON/include/JSON.hpp
    ./JSON/include/JSONSources.hpp
    ./JSON/include/JSONDestinations.hpp
)

# H4 library

add_library(${H4_LIBRARY_NAME} STATIC ${H4_SOURCES})
target_include_directories(${H4_LIBRARY_NAME} PUBLIC ./Bencode/include ./JSON/include ${PROJECT_BINARY_DIR})
target_link_libraries(${H4_LIBRARY_NAME})