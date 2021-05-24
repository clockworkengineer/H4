cmake_minimum_required(VERSION 3.17.0)

project("XML Unit Tests" VERSION 0.0.1 DESCRIPTION "XML C++ Library Catch Unit Tests")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

set(TEST_EXECUTABLE ${XML_LIBRARY_NAME}_Unit_Tests)

set(TEST_SOURCES
    XML_helper.cpp
    ISource.cpp
    IDestination.cpp
    XML_parse_misc.cpp
    XML_parse_declaration.cpp
    XML_parse_tags.cpp
    XML_parse_root.cpp
    XML_parse_files.cpp
    XML_parse_attributes.cpp
    XML_parse_comments.cpp
    XML_parse_unicode.cpp
    XML_parse_entities.cpp
    XML_parse_pi.cpp
    XML_parse_cdata.cpp
    XML_parse_namespace.cpp
    DTD_parse.cpp
    XML_stringify.cpp
    DTD_stringify.cpp
    DTD_validate.cpp
)

add_executable(${TEST_EXECUTABLE} ${TEST_SOURCES})
target_include_directories(${TEST_EXECUTABLE} PUBLIC ../include)
target_link_libraries(${TEST_EXECUTABLE} ${XML_LIBRARY_NAME} Catch2::Catch2WithMain)

add_test(NAME ${TEST_EXECUTABLE} COMMAND ${TEST_EXECUTABLE})

add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/testData testData)


