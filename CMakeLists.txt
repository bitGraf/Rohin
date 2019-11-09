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
set (
    source_list_unused
    "${SRC_DIR}/main.cpp"

    "${SRC_DIR}/Core Systems/CoreSystem.hpp"
    "${SRC_DIR}/Core Systems/CoreSystem.cpp"
    "${SRC_DIR}/Core Systems/Console.hpp"
    "${SRC_DIR}/Core Systems/Console.cpp"
    "${SRC_DIR}/Core Systems/FileSystem.hpp"
    "${SRC_DIR}/Core Systems/FileSystem.cpp"
    "${SRC_DIR}/Core Systems/RenderManager.hpp"
    "${SRC_DIR}/Core Systems/RenderManager.cpp"
    "${SRC_DIR}/Core Systems/ResourceManager.hpp"
    "${SRC_DIR}/Core Systems/ResourceManager.cpp"
    "${SRC_DIR}/Core Systems/SceneManager.hpp"
    "${SRC_DIR}/Core Systems/SceneManager.cpp"
    "${SRC_DIR}/Core Systems/Window.hpp"
    "${SRC_DIR}/Core Systems/Window.cpp"

    "${SRC_DIR}/Message Architechture/Message.hpp"
    "${SRC_DIR}/Message Architechture/Message.cpp"
    "${SRC_DIR}/Message Architechture/MessageBus.hpp"
    "${SRC_DIR}/Message Architechture/MessageBus.cpp"
    "${SRC_DIR}/Message Architechture/Configuration.hpp"
    "${SRC_DIR}/Message Architechture/Configuration.cpp"

    "${SRC_DIR}/Render/Camera.hpp"
    "${SRC_DIR}/Render/Camera.cpp"
    "${SRC_DIR}/Render/Material.hpp"
    "${SRC_DIR}/Render/Material.cpp"
    "${SRC_DIR}/Render/Shader.hpp"
    "${SRC_DIR}/Render/Shader.cpp"
    "${SRC_DIR}/Render/Models.hpp"
    "${SRC_DIR}/Render/tinygltf.cpp"

    "${SRC_DIR}/DataTypes.hpp"
    "${SRC_DIR}/Entity.hpp"
    "${SRC_DIR}/Entity.cpp"
    "${SRC_DIR}/GameMath.hpp"
    "${SRC_DIR}/GameMath.cpp"
    "${SRC_DIR}/HashTable.hpp"
    "${SRC_DIR}/HashTable.cpp"
    "${SRC_DIR}/MemoryManager.hpp"
    "${SRC_DIR}/MemoryManager.cpp"
    "${SRC_DIR}/Platform.hpp"
    "${SRC_DIR}/Platform.cpp"
)

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
assign_source_group(${source_list_game})

set(OPTICK_DIR ${LIB_DIR}/optick)
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

# optick
file(GLOB OPTICK_FILES ${OPTICK_DIR}/src/*.*)
add_library(optick STATIC ${OPTICK_FILES})
set_target_properties(optick PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(Game PRIVATE ${OPTICK_DIR}/src/)
target_link_libraries(Game optick ${CMAKE_DL_LIBS})

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


set_target_properties(tinygltf optick glad glfw PROPERTIES FOLDER Libraries)
set_target_properties(Game engine doc_dox PROPERTIES FOLDER Rohin)
set(VS_STARTUP_PROJECT Game)