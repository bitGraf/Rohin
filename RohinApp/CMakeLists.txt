message(STATUS "[RohinApp]")
file(GLOB_RECURSE SrcFiles "src/*.cpp" "src/*.h")
foreach(file ${SrcFiles})
    message(TRACE "  Found File: '${file}'")
endforeach()

add_executable(RohinApp ${SrcFiles})
set_target_properties(RohinApp PROPERTIES OUTPUT_NAME "${GAME_NAME}")
message(STATUS "Creating Executable: '${GAME_NAME}.exe'")

target_link_libraries(RohinApp PUBLIC Engine)
target_include_directories(RohinApp PRIVATE 
    "src"
    "../Engine/src"
    "../Game/src"
    "../Engine/deps/math_lib/include"
    "../Engine/deps/imgui"
)

target_compile_features(RohinApp PUBLIC cxx_std_14)
add_dependencies(RohinApp GameLib)

target_compile_definitions(RohinApp PRIVATE
    _CRT_SECURE_NO_WARNINGS 
    RH_INTERNAL 
)

set_target_properties(
    RohinApp PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")


message(STATUS "Compile Flags: [${CMAKE_CXX_FLAGS_DEBUG}]")
message(STATUS "Build Type: [${CMAKE_BUILD_TYPE}]")

message(STATUS "[RohinApp configured]")
