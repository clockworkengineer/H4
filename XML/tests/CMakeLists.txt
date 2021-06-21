cmake_minimum_required(VERSION 3.16.0)

project("XML Unit Tests" VERSION 0.0.1 DESCRIPTION "XML C++ Library Catch Unit Tests")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

set(TEST_EXECUTABLE ${XML_LIBRARY_NAME}_Unit_Tests)

set(TEST_SOURCES
    ./IO/ISource.cpp
    ./IO/IDestination.cpp
    ./XML/XML_parse_misc.cpp
    ./XML/XML_parse_declaration.cpp
    ./XML/XML_parse_tags.cpp
    ./XML/XML_parse_root.cpp
    ./XML/XML_parse_files.cpp
    ./XML/XML_parse_attributes.cpp
    ./XML/XML_parse_comments.cpp
    ./XML/XML_parse_unicode.cpp
    ./XML/XML_parse_entities.cpp
    ./XML/XML_parse_pi.cpp
    ./XML/XML_parse_cdata.cpp
    ./XML/XML_parse_namespace.cpp
    ./XML/XML_stringify.cpp
    ./XML/XML_helper.cpp
    ./DTD/DTD_parse_misc.cpp
    ./DTD/DTD_parse_entity.cpp
    ./DTD/DTD_parse_attributes.cpp
    ./DTD/DTD_parse_tags.cpp
    ./DTD/DTD_stringify.cpp
    ./DTD/DTD_validate_elements.cpp
    ./DTD/DTD_validate_attributes.cpp
)

add_executable(${TEST_EXECUTABLE} ${TEST_SOURCES})
target_include_directories(${TEST_EXECUTABLE} PUBLIC ./include ../include)
target_link_libraries(${TEST_EXECUTABLE} ${XML_LIBRARY_NAME} Catch2::Catch2WithMain stdc++fs)

add_test(NAME ${TEST_EXECUTABLE} COMMAND ${TEST_EXECUTABLE})

add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/testData testData)


