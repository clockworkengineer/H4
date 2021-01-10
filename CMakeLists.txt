cmake_minimum_required(VERSION 3.17.0)

project("H4 C++ library" VERSION 0.0.0 DESCRIPTION "H4 C++ Static Library")

set(H4_LIBRARY_NAME H4)

# Enable unit tests

enable_testing()

add_subdirectory(Bencode)
add_subdirectory(JSON)
