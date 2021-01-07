cmake_minimum_required(VERSION 3.17.0)

project("JSON C++ library" VERSION 0.0.0 DESCRIPTION "JSON C++ Static Library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

set(JSON_LIBRARY_NAME JSON)

# Enable unit tests

enable_testing()

# JSON sources and includes

set (JSON_SOURCES
    ./classes/JSON.cpp
)

set (JSON_INCLUDES
    ./include/JSON.hpp
)

# JSON library

add_library(${JSON_LIBRARY_NAME} STATIC ${JSON_SOURCES})
target_include_directories(${JSON_LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${JSON_LIBRARY_NAME})

add_subdirectory(tests)


