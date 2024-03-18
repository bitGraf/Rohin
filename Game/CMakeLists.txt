message(STATUS "[GameLib]")
file(GLOB_RECURSE SrcFiles "src/*.cpp" "src/*.h")
foreach(file ${SrcFiles})
    message(TRACE "  Found File: '${file}'")
endforeach()

set(GAME_NAME "game")
set(GAME_NAME ${GAME_NAME} PARENT_SCOPE)

add_library(GameLib SHARED ${SrcFiles})
set_target_properties(GameLib PROPERTIES OUTPUT_NAME "lib${GAME_NAME}")
message(STATUS "Creating GameLib: 'lib${GAME_NAME}.dll'")

target_link_libraries(GameLib PUBLIC Engine)
target_include_directories(GameLib PRIVATE 
    "src"
    "../Engine/src"
    "../RohinApp/src"
    "../Engine/deps/math_lib/include"
    "../Engine/deps/imgui"
)

target_compile_features(GameLib PUBLIC cxx_std_14)

target_compile_definitions(GameLib PRIVATE
    _CRT_SECURE_NO_WARNINGS 
    GAME_EXPORT
)

set_target_properties(
    GameLib PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")

message(STATUS "Compile Flags: [${CMAKE_CXX_FLAGS_DEBUG}]")
message(STATUS "Build Type: [${CMAKE_BUILD_TYPE}]")

message(STATUS "[GameLib configured]")
