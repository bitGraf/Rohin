message(STATUS "[RohinGame]")
file(GLOB_RECURSE SrcFiles "src/*.cpp" "src/*.h")
foreach(file ${SrcFiles})
    message(TRACE "  Found File: '${file}'")
endforeach()

add_executable(game ${SrcFiles})

target_link_libraries(game PUBLIC rohin)
target_include_directories(game PRIVATE 
    "src"
    "../Engine/src"
    "../Game/src"
    "../Engine/deps/math_lib/include"
    "../Engine/deps/imgui"
)

target_compile_features(game PUBLIC cxx_std_14)

target_compile_definitions(game PUBLIC
    _CRT_SECURE_NO_WARNINGS 
    RH_INTERNAL 
)

set_target_properties(
    game PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")


message(STATUS "RohinGame configured")
