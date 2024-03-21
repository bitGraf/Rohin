message(STATUS "[Engine]")

file(GLOB_RECURSE SrcFiles "src/*.cpp" "src/*.c" "src/*.h")
foreach(file ${SrcFiles})
    message(TRACE "  Found File: '${file}'")
endforeach()

add_subdirectory(shader_tool)

add_library(Engine SHARED ${SrcFiles})
set_target_properties(Engine PROPERTIES OUTPUT_NAME "rohin")
message(STATUS "Creating EngineLib: 'rohin.dll'")

target_link_libraries(Engine PUBLIC opengl32 Winmm Shlwapi)
target_include_directories(Engine PRIVATE 
    "src"
    "deps/math_lib/include"
    "deps/stb"
    "deps/imgui/imgui"
)

add_dependencies(Engine shader_tool_run)

target_compile_features(Engine PUBLIC cxx_std_14)

target_compile_definitions(Engine PRIVATE
    _CRT_SECURE_NO_WARNINGS 
    RH_EXPORT 
    RH_INTERNAL 
    RH_PLATFORM_WINDOWS 
    RH_COMPILE_OPENGL_4_4
)

if(MSVC)
  target_compile_options(Engine PRIVATE /W4 /WX /wd4201 /wd4189 /wd4100)
else()
  target_compile_options(Engine PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

message(STATUS "Compile Flags: [${CMAKE_CXX_FLAGS_DEBUG}]")
message(STATUS "Build Type: [${CMAKE_BUILD_TYPE}]")

message(STATUS "[Engine configured]")
