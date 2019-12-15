cmake_minimum_required(VERSION 3.6)
set(ENGINE_LIB_NAME "Engine")

function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}/" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

file(GLOB_RECURSE  engine_HEADERS "*.hpp")
file(GLOB_RECURSE  engine_SOURCES "*.cpp")
set_source_files_properties(${engine_SOURCES} PROPERTIES LANGUAGE CXX)

add_library(engine ${engine_SOURCES} ${engine_HEADERS})
assign_source_group(${engine_SOURCES})
assign_source_group(${engine_HEADERS})
set_target_properties(engine PROPERTIES
   OUTPUT_NAME ${ENGINE_LIB_NAME}
   VERSION 0.1.0
   SOVERSION 0
   POSITION_INDEPENDENT_CODE OFF)
#   FOLDER "Rohin")

target_include_directories(engine PRIVATE
        "${ENGINE_DIR}")

target_compile_definitions(engine PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_definitions(engine PRIVATE "GLFW_INCLUDE_NONE")

option(ENGINE_CUSTOM_ENTITIES "ayo" OFF)
if(ENGINE_CUSTOM_ENTITIES)
    target_compile_definitions(engine PRIVATE "CUSTOM_ENTITIES")
else()
endif(ENGINE_CUSTOM_ENTITIES)


target_include_directories(engine PRIVATE ${GLAD_DIR}/include/)
target_include_directories(engine PRIVATE ${GLFW_DIR}/include/)
target_include_directories(engine PRIVATE ${TINYGLTF_DIR}/)