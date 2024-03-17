message(STATUS "[shader_tool]")

set(SrcFiles "shader_tool.cpp")

add_executable(shader_tool ${SrcFiles})

#target_link_libraries(shader_tool PUBLIC opengl32 Winmm)

target_compile_features(shader_tool PUBLIC cxx_std_14)

target_compile_definitions(shader_tool PUBLIC
    _CRT_SECURE_NO_WARNINGS 
)

if(MSVC)
  target_compile_options(shader_tool PRIVATE /W4 /WX /wd4201 /wd4189 /wd4100)
else()
  target_compile_options(shader_tool PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

ADD_CUSTOM_TARGET(shader_tool_run ALL
                 bin/shader_tool.exe "Game\\run_tree\\Data\\Shaders\\" "Engine\\src\\Engine\\Renderer\\ShaderSrc\\shaders_generated" "-quiet"
                 DEPENDS shader_tool
                 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                 COMMENT "Generating shader source using shader_tool.exe"
                 VERBATIM)

message(STATUS "shader_tool configured")
