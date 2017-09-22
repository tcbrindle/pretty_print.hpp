
cmake_minimum_required(VERSION 3.1)

project(pretty_print CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS Off)

add_library(pretty_print INTERFACE)
target_include_directories(pretty_print INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}}/include)
target_sources(pretty_print INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/tcb/pretty_print.hpp)

add_subdirectory(test)

