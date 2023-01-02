set(ENGINE_NAME Engine)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (ROHIN_PLATFORM STREQUAL "Win32_64")
    message(STATUS "ENGINE:: Generating code using Win32 x64 Platform layer")
    set(PLATFORM_SRC src/Platform/win32_main.cpp)
else()
    message(FATAL_ERROR "Unsupported Platform: [${ROHIN_PLATFORM}]!")
endif()

if (ROHIN_RENDERER STREQUAL "OpenGL")
    message(STATUS "ENGINE:: Generating code using OpenGL")
else()
    message(FATAL_ERROR "Unsupported Renderer: [${ROHIN_RENDERER}]!")
endif()

# Engine dependencies
#add_subdirectory(deps/spdlog)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#add_subdirectory(deps/GLFW/GLFW)
#add_subdirectory(deps/GLAD)
#add_subdirectory(deps/imgui)
#add_subdirectory(deps/OpenAL_soft)
#add_subdirectory(deps/stb)
#add_subdirectory(deps/math_lib)

set(ANIMATION_SRC
    # src/Engine/Collision
    src/Engine/Animation/Animation.cpp
    src/Engine/Animation/Animation.hpp
)
set(CORE_SRC 
    # src/Engine/Core
    src/Engine/Core/Application.cpp
    src/Engine/Core/Application.hpp
    src/Engine/Core/Assert.hpp
    src/Engine/Core/Base.hpp
    src/Engine/Core/DataFile.hpp
    src/Engine/Core/DataTypes.hpp
    src/Engine/Core/Input.hpp
    src/Engine/Core/KeyCodes.hpp
    src/Engine/Core/Logger.cpp
    src/Engine/Core/Logger.hpp
    src/Engine/Core/MemoryTrack.cpp
    src/Engine/Core/MemoryTrack.hpp
    src/Engine/Core/MouseButtonCodes.hpp
    src/Engine/Core/Platform.hpp
    src/Engine/Core/Timing.hpp
    src/Engine/Core/Utils.cpp
    src/Engine/Core/Utils.hpp
    src/Engine/Core/Window.hpp
)
set(COLLISION_SRC
    # src/Engine/Collision
    src/Engine/Collision/CollisionHull.cpp
    src/Engine/Collision/CollisionHull.hpp
    src/Engine/Collision/CollisionWorld.cpp
    src/Engine/Collision/CollisionWorld.hpp
)
set(EVENT_SRC 
    # src/Engine/Event
    src/Engine/Event/Event.hpp
    src/Engine/Event/EventTypes.hpp
)
set(EXAMPLES_SRC
    # src/Engine/Examples
    src/Engine/Examples/ExampleLevels.cpp
    src/Engine/Examples/ExampleLevels.hpp
)
set(GAMEOBJECT_SRC
    # src/Engine/GameObject
    src/Engine/GameObject/Components.hpp
    src/Engine/GameObject/GameObject.hpp
    src/Engine/GameObject/ScriptableBase.hpp
)
set(PLATFORM_OPENAL_SRC
    # src/Engine/Platform/OpenAL
    src/Engine/Platform/OpenAL/OpenALBuffer.cpp
    src/Engine/Platform/OpenAL/OpenALBuffer.hpp
    src/Engine/Platform/OpenAL/OpenALContext.cpp
    src/Engine/Platform/OpenAL/OpenALContext.hpp
    src/Engine/Platform/OpenAL/OpenALDevice.cpp
    src/Engine/Platform/OpenAL/OpenALDevice.h
    src/Engine/Platform/OpenAL/OpenALEffects.cpp
    src/Engine/Platform/OpenAL/OpenALError.cpp
    src/Engine/Platform/OpenAL/OpenALError.hpp
    src/Engine/Platform/OpenAL/OpenALSoundStream.cpp
    src/Engine/Platform/OpenAL/OpenALSoundStream.hpp
    src/Engine/Platform/OpenAL/OpenALSource.cpp
    src/Engine/Platform/OpenAL/OpenALSource.hpp
)
set(PLATFORM_OPENGL_SRC
    # src/Engine/Platform/OpenGL
    src/Engine/Platform/OpenGL/OpenGLBuffer.cpp
    src/Engine/Platform/OpenGL/OpenGLBuffer.hpp
    src/Engine/Platform/OpenGL/OpenGLFramebuffer.cpp
    src/Engine/Platform/OpenGL/OpenGLFramebuffer.hpp
    src/Engine/Platform/OpenGL/OpenGLGraphicsContext.cpp
    src/Engine/Platform/OpenGL/OpenGLGraphicsContext.hpp
    src/Engine/Platform/OpenGL/OpenGLRendererAPI.cpp
    src/Engine/Platform/OpenGL/OpenGLRendererAPI.hpp
    src/Engine/Platform/OpenGL/OpenGLShader.cpp
    src/Engine/Platform/OpenGL/OpenGLShader.hpp
    src/Engine/Platform/OpenGL/OpenGLTexture.cpp
    src/Engine/Platform/OpenGL/OpenGLTexture.hpp
    src/Engine/Platform/OpenGL/OpenGLVertexArray.cpp
    src/Engine/Platform/OpenGL/OpenGLVertexArray.hpp
)
set(PLATFORM_WINDOWS_SRC
    # src/Engine/Platform/Windows
    src/Engine/Platform/Windows/Input_windows.cpp
    src/Engine/Platform/Windows/Window_windows.cpp
    src/Engine/Platform/Windows/Window_windows.hpp
)
set(RENDERER_SRC
    # src/Engine/Renderer
    src/Engine/Renderer/Buffer.cpp
    src/Engine/Renderer/Buffer.hpp
    src/Engine/Renderer/Camera.hpp
    src/Engine/Renderer/Framebuffer.cpp
    src/Engine/Renderer/Framebuffer.hpp
    src/Engine/Renderer/GraphicsContext.hpp
    src/Engine/Renderer/Light.hpp
    src/Engine/Renderer/Material.cpp
    src/Engine/Renderer/Material.hpp
    src/Engine/Renderer/Mesh.cpp
    src/Engine/Renderer/Mesh.hpp
    src/Engine/Renderer/RenderCommand.cpp
    src/Engine/Renderer/RenderCommand.hpp
    src/Engine/Renderer/Renderer.cpp
    src/Engine/Renderer/Renderer.hpp
    src/Engine/Renderer/RendererAPI.cpp
    src/Engine/Renderer/RendererAPI.hpp
    src/Engine/Renderer/Shader.cpp
    src/Engine/Renderer/Shader.hpp
    src/Engine/Renderer/SpriteRenderer.cpp
    src/Engine/Renderer/SpriteRenderer.hpp
    src/Engine/Renderer/TextRenderer.cpp
    src/Engine/Renderer/TextRenderer.hpp
    src/Engine/Renderer/Texture.cpp
    src/Engine/Renderer/Texture.hpp
    src/Engine/Renderer/VertexArray.cpp
    src/Engine/Renderer/VertexArray.hpp
)
set(RESOURCES_NBT_SRC
    # src/Engine/Resources/nbt
    src/Engine/Resources/nbt/data.hpp
    src/Engine/Resources/nbt/endian.cpp
    src/Engine/Resources/nbt/endian.hpp
    src/Engine/Resources/nbt/io.cpp
    src/Engine/Resources/nbt/io.hpp
    src/Engine/Resources/nbt/nbt.cpp
    src/Engine/Resources/nbt/nbt.hpp
    src/Engine/Resources/nbt/tag.cpp
    src/Engine/Resources/nbt/tag.hpp
    src/Engine/Resources/nbt/test.hpp
    src/Engine/Resources/nbt/utils.hpp
)
set(RESOURCES_SRC
    # src/Engine/Resources
    src/Engine/Resources/Catalog.cpp
    src/Engine/Resources/Catalog.hpp
    src/Engine/Resources/DynamicFont.cpp
    src/Engine/Resources/DynamicFont.hpp
    src/Engine/Resources/MaterialCatalog.cpp
    src/Engine/Resources/MaterialCatalog.hpp
    src/Engine/Resources/MeshCatalog.cpp
    src/Engine/Resources/MeshCatalog.hpp
    src/Engine/Resources/ResourceManager.cpp
    src/Engine/Resources/ResourceManager.hpp
)
set(SCENE_SRC
    # src/Engine/Scene
    src/Engine/Scene/Scene.hpp
    src/Engine/Scene/Scene3D.cpp
    src/Engine/Scene/Scene3D.hpp
    src/Engine/Scene/SceneCamera.cpp
    src/Engine/Scene/SceneCamera.hpp
)
set(GUI_SRC
    # src/Engine/Gui
    src/Engine/Gui/GuiLayer.cpp
    src/Engine/Gui/GuiLayer.hpp
)
set(SCRIPTS_SRC
    # src/Engine/Scripts
    src/Engine/Scripts/CameraController.hpp
    src/Engine/Scripts/Player.hpp
)
set(SOUND_SRC
    # src/Engine/Sound
    src/Engine/Sound/SoundBuffer.hpp
    src/Engine/Sound/SoundContext.hpp
    src/Engine/Sound/SoundDevice.hpp
    src/Engine/Sound/SoundEffect.hpp
    src/Engine/Sound/SoundEngine.cpp
    src/Engine/Sound/SoundEngine.hpp
    src/Engine/Sound/SoundFileFormats.cpp
    src/Engine/Sound/SoundFileFormats.hpp
    src/Engine/Sound/SoundSource.hpp
    src/Engine/Sound/SoundStream.hpp
)
set(ENGINE_SRC
    src/Engine/Benchmark.cpp
    src/Engine/Benchmark.hpp
    src/Engine/Buffer.hpp
    src/Engine/EntryPoint.h
    src/Engine/laml_impl.cpp
)
set(ALL_SRC
    # src/Engine
    src/Engine.hpp
    src/enpch.cpp
    src/enpch.hpp
    ${ENGINE_SRC}
    ${ANIMATION_SRC}
    ${COLLISION_SRC}
    ${CORE_SRC}
    ${EVENT_SRC}
    ${EXAMPLES_SRC}
    ${GAMEOBJECT_SRC}
    ${GUI_SRC}
    ${PLATFORM_OPENAL_SRC}
    ${PLATFORM_OPENGL_SRC}
    ${PLATFORM_WINDOWS_SRC}
    ${RENDERER_SRC}
    ${RESOURCES_SRC}
    ${RESOURCES_NBT_SRC}
    ${SCENE_SRC}
    ${SCRIPTS_SRC}
    ${SOUND_SRC}
)

if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:WinMainCRTStartup" )
    message(STATUS "Linker flags: ${CMAKE_EXE_LINKER_FLAGS}")
endif()

# add_library(${ENGINE_NAME} STATIC)
add_executable(${ENGINE_NAME})
target_sources(${ENGINE_NAME} PRIVATE ${PLATFORM_SRC})
target_include_directories(${ENGINE_NAME} PUBLIC src/)
target_compile_features(${ENGINE_NAME} PUBLIC cxx_std_17)
target_compile_definitions(${ENGINE_NAME} PUBLIC HANDMADE_SLOW=1)
target_compile_definitions(${ENGINE_NAME} PUBLIC HANDMADE_INTERNAL=1)
target_link_libraries(${ENGINE_NAME} user32.lib Gdi32.lib Winmm.lib opengl32.lib)

#source_group(Src FILES src/Engine.hpp src/enpch.cpp src/enpch.hpp)
#source_group(Src\\Engine FILES ${ENGINE_SRC})
#source_group(Src\\Engine\\Animation FILES ${ANIMATION_SRC})
#source_group(Src\\Engine\\Collision FILES ${COLLISION_SRC})
#source_group(Src\\Engine\\Core FILES ${CORE_SRC})
#source_group(Src\\Engine\\Event FILES ${EVENT_SRC})
#source_group(Src\\Engine\\Examples FILES ${EXAMPLES_SRC})
#source_group(Src\\Engine\\GameObject FILES ${GAMEOBJECT_SRC})
#source_group(Src\\Engine\\Gui FILES ${GUI_SRC})
#source_group(Src\\Engine\\Platform FILES ${PLATFORM_SRC})
#source_group(Src\\Engine\\Platform\\OpenAL FILES ${PLATFORM_OPENAL_SRC})
#source_group(Src\\Engine\\Platform\\OpenGL FILES ${PLATFORM_OPENGL_SRC})
#source_group(Src\\Engine\\Platform\\Windows FILES ${PLATFORM_WINDOWS_SRC})
#source_group(Src\\Engine\\Renderer FILES ${RENDERER_SRC})
#source_group(Src\\Engine\\Resources FILES ${RESOURCES_SRC})
#source_group(Src\\Engine\\Resources\\nbt FILES ${RESOURCES_NBT_SRC})
#source_group(Src\\Engine\\Scene FILES ${SCENE_SRC})
#source_group(Src\\Engine\\Scripts FILES ${SCRIPTS_SRC})
#source_group(Src\\Engine\\Sound FILES ${SOUND_SRC})

#target_link_libraries(${ENGINE_NAME} PUBLIC spdlog imgui OpenAL_Soft stb OpenAL32 opengl32 laml)
#target_include_directories(${ENGINE_NAME} PUBLIC src/)
#target_include_directories(${ENGINE_NAME} PUBLIC deps/EnTT/src)
#target_compile_features(${ENGINE_NAME} PUBLIC cxx_std_17)
#target_compile_definitions(${ENGINE_NAME} PRIVATE GLFW_INCLUDE_NONE)
#
#set_target_properties(glad imgui OpenAL_Soft spdlog stb PROPERTIES FOLDER external)