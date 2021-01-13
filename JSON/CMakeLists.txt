cmake_minimum_required(VERSION 3.17.0)

project("JSON" VERSION 0.0.1 DESCRIPTION "JSON C++ Static Library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

configure_file(JSONConfig.h.in JSONConfig.hpp)

set(JSON_LIBRARY_NAME ${PROJECT_NAME})

# Enable unit tests

enable_testing()

# JSON sources and includes

set (JSON_SOURCES
    ./classes/JSON.cpp
)

set (JSON_INCLUDES
    JSONCOnfig.hpp
    ./include/JSON.hpp
    ./include/JSONSources.hpp
    ./include/JSONDestinations.hpp
)

# JSON library

add_library(${JSON_LIBRARY_NAME} STATIC ${JSON_SOURCES})
target_include_directories(${JSON_LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR} )
target_link_libraries(${JSON_LIBRARY_NAME})

# Add tests

add_subdirectory(tests)

# install

install(TARGETS ${JSON_LIBRARY_NAME} DESTINATION lib)
install(FILES JSON.hpp JSONConfig.hpp DESTINATION include)


