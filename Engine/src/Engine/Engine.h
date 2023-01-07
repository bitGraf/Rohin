#ifndef ENGINE_H
#define ENGINE_H

#include "Engine/Core/MemoryArena.hpp"
#include "Engine/Renderer/CommandBuffer.h"
#include "Engine/Renderer/Renderer.hpp"

/* Services that the Engine provides to the Game: */

// Logging
#define ENGINE_LOG_MESSAGE(name) void name(const char* msg)
typedef ENGINE_LOG_MESSAGE(EngineLogMessage_t);

// Filesystem
#define ENGINE_GET_EXE_PATH(name) const char* name(void)
typedef ENGINE_GET_EXE_PATH(EngineGetEXEPath_t);

// Renderer
#define ENGINE_RENDER_LOAD_SHADER(name) bool32 name(Shader* shader, char* ShaderPath)
typedef ENGINE_RENDER_LOAD_SHADER(EngineRenderLoadShader_t);

#define ENGINE_RENDER_BEGIN_FRAME(name) void name(void)
typedef ENGINE_RENDER_BEGIN_FRAME(EngineRenderBeginFrame_t);

#define ENGINE_RENDER_END_FRAME(name) void name(void)
typedef ENGINE_RENDER_END_FRAME(EngineRenderEndFrame_t);

/*
vertex_buffer OpenGLCreateVertexBuffer(void* VertexData, uint32 DataSizeInBytes);
void OpenGLSetVertexBufferLayout(uint32 NumAttributes, ShaderDataType first, ...);
index_buffer OpenGLCreateIndexBuffer(uint32* IndexData, uint32 IndexCount);
vertex_array_object OpenGLCreateVertexArray(vertex_buffer* VBO, index_buffer* IBO);
 * */
typedef vertex_buffer EngineRenderCreateVertexBuffer_t(void*, uint32);
typedef void EngineRenderSetVertexBufferLayout_t(vertex_buffer*, uint8, ShaderDataType, ...);
typedef index_buffer EngineRenderCreateIndexBuffer_t(uint32*, uint32);
typedef vertex_array_object EngineRenderCreateVertexArray_t(vertex_buffer*, index_buffer*);

typedef bool32 EngineRenderLoadDynamicFont_t(dynamic_font*, char*, real32, uint32);

//Engine.GetTextOffset(&GameState->TextRenderer.Font, &HOffset, &VOffset, Alignment, Text);
#define ENGINE_GET_TEXT_OFFSET(name) void name(dynamic_font* Font, real32* HOffset, real32* VOffset, TextAlignment Alignment, const char* Text)
typedef ENGINE_GET_TEXT_OFFSET(EngineGetTextOffset_t);

struct gameImport_t {
    int Version;

    // Logging stuff
    EngineLogMessage_t* LogMessage;
    
    // Filesystem stuff
    EngineGetEXEPath_t* GetEXEPath;

    // Renderer stuff
    EngineRenderLoadShader_t* LoadShader;

    EngineRenderCreateVertexBuffer_t *CreateVertexBuffer;
    EngineRenderSetVertexBufferLayout_t *SetVertexBufferLayout;
    EngineRenderCreateIndexBuffer_t *CreateIndexBuffer;
    EngineRenderCreateVertexArray_t *CreateVertexArray;

    EngineRenderLoadDynamicFont_t *LoadDynamicFont;

    EngineGetTextOffset_t* GetTextOffset;
};

#endif