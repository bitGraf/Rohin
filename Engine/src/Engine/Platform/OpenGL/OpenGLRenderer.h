#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include "Engine/Core/Base.hpp"
#include "Engine/Renderer/CommandBuffer.h"

internal_func void OpenGLBeginFrame(win32_window_dimension* Dimension);
internal_func void OpenGLEndFrame(render_command_buffer* CommandBuffer);

#endif