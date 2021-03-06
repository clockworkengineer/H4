cmake_minimum_required(VERSION 3.16.0)

project("JSON" VERSION 0.1.0 DESCRIPTION "JSON C++ Static Library")

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
    ./classes/JSONTranslator.cpp
)

set (JSON_INCLUDES
    JSONConfig.hpp
    ./include/JSON.hpp
    ./include/JNode.hpp
    ./include/JSONSources.hpp
    ./include/JSONDestinations.hpp
    ./include/JSONTranslator.hpp
)

# JSON library

add_library(${JSON_LIBRARY_NAME} STATIC ${JSON_SOURCES})
target_include_directories(${JSON_LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR} )
target_link_libraries(${JSON_LIBRARY_NAME} stdc++fs)

# Add tests

add_subdirectory(tests)

# install

install(TARGETS ${JSON_LIBRARY_NAME} DESTINATION lib)
install(FILES JSON.hpp JSONConfig.hpp JNode.hpp DESTINATION include)


