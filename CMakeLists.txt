cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0048 NEW)

project(rohin 
    VERSION 1.0
    DESCRIPTION "Example project with CMake"
    LANGUAGES CXX)
    set(VS_STARTUP_PROJECT bigDisc)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Find all files
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.hpp")

list(REMOVE_ITEM SOURCES "src/testing.cpp")

#Bring headers into project
#include_directories(include)
add_executable(bigDisc ${SOURCES} ${HEADERS})
target_include_directories(bigDisc PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(testing ${CMAKE_SOURCE_DIR}/src/testing.cpp)
target_include_directories(testing PRIVATE ${CMAKE_SOURCE_DIR}/src)