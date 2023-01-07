#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include "Engine/Core/Base.hpp"
#include "Engine/Renderer/CommandBuffer.h"
#include "Engine/Renderer/Renderer.hpp"

internal_func bool32 OpenGLLoadShader(shader* shader, uint8* Buffer, uint32 BytesRead);

internal_func vertex_buffer OpenGLCreateVertexBuffer(void* VertexData, uint32 DataSizeInBytes, uint8 NumAttributes, ShaderDataType First, ...);
internal_func index_buffer OpenGLCreateIndexBuffer(uint32* IndexData, uint32 IndexCount);
internal_func vertex_array_object OpenGLCreateVertexArray(vertex_buffer* VBO, index_buffer* IBO);

internal_func uint32 OpenGLCreateTexture(uint8* Bitmap, uint32 Resolution);

internal_func void OpenGLBeginFrame();
internal_func void OpenGLEndFrame(win32_window_dimension* Dimension, render_command_buffer* CommandBuffer);

#endif