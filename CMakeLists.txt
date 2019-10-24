cmake_minimum_required(VERSION 3.1)
#cmake_policy(SET CMP0048 NEW)

project(rohin 
    VERSION 0.1
    DESCRIPTION "Second Attempt"
    LANGUAGES CXX)
set(VS_STARTUP_PROJECT ${PROJECT_NAME})

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Find all source files
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps)
file(GLOB SOURCES ${SRC_DIR}/*.cpp)
file(GLOB HEADERS ${SRC_DIR}/*.hpp)

# Executable definition
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

# GLFW
set(GLFW_DIR ${LIB_DIR}/glfw)
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory(${GLFW_DIR})
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_DIR}/include)
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# glad
set(GLAD_DIR ${LIB_DIR}/glad)
add_library(glad ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PRIVATE ${GLAD_DIR}/include/)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLAD_DIR}/include/)
target_link_libraries(${PROJECT_NAME} glad ${CMAKE_DL_LIBS})

# optick
set(OPTICK_DIR ${LIB_DIR}/optick)
file(GLOB OPTICK_FILES ${OPTICK_DIR}/src/*.*)
add_library(optick STATIC ${OPTICK_FILES})
set_target_properties(optick PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${PROJECT_NAME} PRIVATE ${OPTICK_DIR}/src/)
target_link_libraries(${PROJECT_NAME} optick ${CMAKE_DL_LIBS})

# tinyglTF
set(TINYGLTF_DIR ${LIB_DIR}/tinygltf)
set(TINYGLTF_FILES
    ${TINYGLTF_DIR}/stb_image.h
    ${TINYGLTF_DIR}/stb_image_write.h
    ${TINYGLTF_DIR}/json.hpp
    ${TINYGLTF_DIR}/tiny_gltf.h
    ${SRC_DIR}/tinygltf.cpp)
add_library(tinygltf STATIC ${TINYGLTF_FILES})
set_target_properties(tinygltf PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${PROJECT_NAME} PRIVATE ${TINYGLTF_DIR}/)
target_include_directories(tinygltf PRIVATE ${TINYGLTF_DIR}/)
target_link_libraries(${PROJECT_NAME} tinygltf ${CMAKE_DL_LIBS})

# Doxygen
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/../docs/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    # add_custom_target(doc_dox ALL
	add_custom_target(doc_dox
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)