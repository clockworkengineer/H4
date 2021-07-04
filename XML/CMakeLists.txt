cmake_minimum_required(VERSION 3.16.0)

project("XML" VERSION 0.0.1 DESCRIPTION "XML C++ Static Library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

configure_file(XMLConfig.h.in XMLConfig.hpp)

set(XML_LIBRARY_NAME ${PROJECT_NAME})

# Enable unit tests

enable_testing()

# XML sources and includes

set (XML_SOURCES
    ./classes/XML.cpp
    ./classes/XML_parse.cpp
    ./classes/XML_stringify.cpp
    ./classes/XML_core.cpp
    ./classes/DTD.cpp
    ./classes/DTD_parse.cpp
    ./classes/DTD_parse_content.cpp
    ./classes/DTD_parse_external.cpp
    ./classes/DTD_validate.cpp
)
 
set (XML_INCLUDES
    XMLConfig.hpp
    ./include/XML.hpp
    ./include/ISource.hpp
    ./inclide/IDestination.hpp
    ./include/XMLSources.hpp
    ./include/XMLDestinations.hpp
    ./include/XMLNode.hpp
    ./incldue/XML_core.hpp
    ./include/DTD.cpp
)

# XML library

add_library(${XML_LIBRARY_NAME} STATIC ${XML_SOURCES})
target_include_directories(${XML_LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR} )
target_link_libraries(${XML_LIBRARY_NAME} stdc++fs )

# Add tests

add_subdirectory(tests)

# install

install(TARGETS ${XML_LIBRARY_NAME} DESTINATION lib)
install(FILES XML.hpp XMLConfig.hpp DESTINATION include)


