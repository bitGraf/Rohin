message(STATUS "[Engine]")

file(GLOB_RECURSE SrcFiles "src/*.cpp" "src/*.c" "src/*.h")
foreach(file ${SrcFiles})
    message(TRACE "  Found File: '${file}'")
endforeach()

add_subdirectory(shader_tool)

add_library(rohin SHARED ${SrcFiles})

target_link_libraries(rohin PUBLIC opengl32 Winmm)
target_include_directories(rohin PRIVATE 
    "src"
    "../Game/src"
    "deps/math_lib/include"
    "deps/stb"
    "deps/imgui/imgui"
)

add_dependencies(rohin shader_tool_run)

target_compile_features(rohin PUBLIC cxx_std_14)

target_compile_definitions(rohin PUBLIC
    RH_EXPORT 
    _CRT_SECURE_NO_WARNINGS 
    ROHIN_INTERNAL 
    ROHIN_WINDOWS 
    RH_COMPILE_OPENGL_4_4
)

if(MSVC)
  target_compile_options(rohin PRIVATE /W4 /WX /wd4201 /wd4189 /wd4100)
else()
  target_compile_options(rohin PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

message(STATUS "Compile Flags: [${CMAKE_CXX_FLAGS_DEBUG}]")
message(STATUS "Build Type: [${CMAKE_BUILD_TYPE}]")

message(STATUS "Engine configured")
