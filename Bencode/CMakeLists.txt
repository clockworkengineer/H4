cmake_minimum_required(VERSION 3.17.0)

project("Bencode" VERSION 0.0.1 DESCRIPTION "Bencode C++ Static Library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

configure_file(BencodeConfig.h.in BencodeConfig.hpp)

set(BENCODE_LIBRARY_NAME "${PROJECT_NAME}")

# Enable unit tests

enable_testing()

# Bencode sources and includes
 
set (BENCODE_SOURCES
    ./classes/Bencode.cpp
)

set (BENCODE_INCLUDES
    ./include/Bencode.hpp
    ./include/BencodeSources.hpp
    ./include/BencodeDestinations.hpp
)

# Bencode library

add_library(${BENCODE_LIBRARY_NAME} STATIC ${BENCODE_SOURCES})
target_include_directories(${BENCODE_LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR})
target_link_libraries(${BENCODE_LIBRARY_NAME})

# Add tests

add_subdirectory(tests)

# install

install(TARGETS ${BENCODE_LIBRARY_NAME} DESTINATION lib)
install(FILES Bencode.hpp BencodeConfig.hpp DESTINATION include)


