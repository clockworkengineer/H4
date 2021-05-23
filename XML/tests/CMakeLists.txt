cmake_minimum_required(VERSION 3.17.0)

project("XML Unit Tests" VERSION 0.0.1 DESCRIPTION "XML C++ Library Catch Unit Tests")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

set(TEST_EXECUTABLE ${XML_LIBRARY_NAME}_Unit_Tests)

set(TEST_SOURCES
    XML_tests_main.cpp
    XML_tests_ISource.cpp
    XML_tests_IDestination.cpp
    XML_tests_parse_misc.cpp
    XML_tests_parse_declaration.cpp
    XML_tests_parse_tags.cpp
    XML_tests_parse_root.cpp
    XML_tests_parse_files.cpp
    XML_tests_parse_attributes.cpp
    XML_tests_parse_comments.cpp
    XML_tests_parse_unicode.cpp
    XML_tests_parse_entities.cpp
    XML_tests_parse_pi.cpp
    XML_tests_parse_cdata.cpp
    XML_tests_parse_namespace.cpp
    XML_tests_dtd_parse.cpp
    XML_tests_stringify.cpp
    XML_tests_dtd_stringify.cpp
    XML_tests_dtd_validate.cpp
)

add_executable(${TEST_EXECUTABLE} ${TEST_SOURCES})
target_include_directories(${TEST_EXECUTABLE} PUBLIC ../include)
target_link_libraries(${TEST_EXECUTABLE} ${XML_LIBRARY_NAME} Catch2::Catch2WithMain)

add_test(NAME ${TEST_EXECUTABLE} COMMAND ${TEST_EXECUTABLE})

add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/testData testData)


