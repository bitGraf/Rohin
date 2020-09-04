cmake_minimum_required(VERSION 3.6)
#cmake_policy(SET CMP0048 NEW)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(Rohin 
    VERSION 0.1
    DESCRIPTION "Second Attempt"
    LANGUAGES C CXX)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Find all source files
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps)

file(GLOB_RECURSE  source_list "src/*.?pp")

function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}/src/" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

set(GLFW_DIR ${LIB_DIR}/glfw)
set(GLAD_DIR ${LIB_DIR}/glad)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory(${GLFW_DIR})

# glad
add_library(glad ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PRIVATE ${GLAD_DIR}/include/)

# Executable definition for Game
add_executable(Game ${source_list})
target_include_directories(Game PRIVATE ${SRC_DIR}/Game/)
set_property(TARGET Game PROPERTY CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
assign_source_group(${source_list})

# Point to Headers
target_include_directories(Game PRIVATE ${SRC_DIR}/Engine)  # Engine library includes
target_include_directories(Game PRIVATE ${SRC_DIR}/Game)  # Game library includes
target_include_directories(Game PRIVATE ${GLFW_DIR}/include) #GLFW includes
target_compile_definitions(Game PRIVATE "GLFW_INCLUDE_NONE") # don't have glfw include openGL bindings, use GLAD instead
target_include_directories(Game PRIVATE ${GLAD_DIR}/include/) # openGL bindings

# Link libraries
target_link_libraries(Game glfw ${GLFW_LIBRARIES})
target_link_libraries(Game glad ${CMAKE_DL_LIBS})
target_link_libraries(Game ${OPENGL_LIBRARIES})

# Assign Visual Studio folders
set_target_properties(glad glfw PROPERTIES FOLDER Libraries)
set_target_properties(Game PROPERTIES FOLDER Rohin)
set(VS_STARTUP_PROJECT Game)

# Install rules
set(RUN_TREE_PATH ${CMAKE_SOURCE_DIR}/run_tree)
install(FILES ${CMAKE_BINARY_DIR}/Game.exe DESTINATION ${RUN_TREE_PATH})
install(FILES ${CMAKE_BINARY_DIR}/glad.lib DESTINATION ${RUN_TREE_PATH})
install(FILES ${CMAKE_BINARY_DIR}/glfw3.lib DESTINATION ${RUN_TREE_PATH})

#add_custom_command(
#    TARGET Game
#    POST_BUILD
#    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target INSTALL
#    )