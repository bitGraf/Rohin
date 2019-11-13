cmake_minimum_required(VERSION 3.1)
#cmake_policy(SET CMP0048 NEW)

project(Rohin 
    VERSION 0.1
    DESCRIPTION "Second Attempt"
    LANGUAGES CXX)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Find all source files
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps)
#file(GLOB source_list ${SRC_DIR}/*)
#file(GLOB HEADERS ${SRC_DIR}/*.hpp)

file(GLOB_RECURSE  source_list_game "src/Game/*.?pp")

function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}/src/Game/" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

# Executable definition for Game
add_executable(Game ${source_list_game})
target_include_directories(Game PRIVATE ${SRC_DIR}/Game/)
set_property(TARGET Game PROPERTY CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
assign_source_group(${source_list_game})

set(GLFW_DIR ${LIB_DIR}/glfw)
set(GLAD_DIR ${LIB_DIR}/glad)
set(TINYGLTF_DIR ${LIB_DIR}/tinygltf)

# Engine
set(ENGINE_DIR ${SRC_DIR}/Engine)
add_subdirectory(${ENGINE_DIR})
target_include_directories(Game PRIVATE ${ENGINE_DIR})
target_link_libraries(Game engine ${CMAKE_DL_LIBS})

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory(${GLFW_DIR})
target_link_libraries(Game glfw ${GLFW_LIBRARIES})
target_include_directories(Game PRIVATE ${GLFW_DIR}/include)
target_compile_definitions(Game PRIVATE "GLFW_INCLUDE_NONE")

# glad
add_library(glad ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PRIVATE ${GLAD_DIR}/include/)
target_include_directories(Game PRIVATE ${GLAD_DIR}/include/)
target_link_libraries(Game glad ${CMAKE_DL_LIBS})

# tinyglTF
set(TINYGLTF_FILES
    ${TINYGLTF_DIR}/stb_image.h
    ${TINYGLTF_DIR}/stb_image_write.h
    ${TINYGLTF_DIR}/json.hpp
    ${TINYGLTF_DIR}/tiny_gltf.h
    ${SRC_DIR}/Engine/tinygltf.cpp)
add_library(tinygltf STATIC ${TINYGLTF_FILES})
set_target_properties(tinygltf PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(Game PRIVATE ${TINYGLTF_DIR}/)
target_include_directories(tinygltf PRIVATE ${TINYGLTF_DIR}/)
target_link_libraries(Game tinygltf ${CMAKE_DL_LIBS})

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


set_target_properties(tinygltf glad glfw PROPERTIES FOLDER Libraries)
set_target_properties(Game engine doc_dox PROPERTIES FOLDER Rohin)
set(VS_STARTUP_PROJECT Game)