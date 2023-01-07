#ifndef ENGINE_H
#define ENGINE_H

#include "Engine/Core/MemoryArena.hpp"
#include "Engine/Renderer/CommandBuffer.h"
#include "Engine/Renderer/Renderer.hpp"

/* Services that the Engine provides to the Game: */
#define ENGINE_LOAD_DYNAMIC_FONT(name) bool32 name(dynamic_font*, char*, real32, uint32)
typedef ENGINE_LOAD_DYNAMIC_FONT(Engine_LoadDynamicFont_t);

// Render Commands
#define ENGINE_RENDER_LOAD_SHADER(name) bool32 name(shader* shader, char* ShaderPath)
typedef ENGINE_RENDER_LOAD_SHADER(Engine_Render_LoadShaderFromFile_t);

#define ENGINE_RENDER_CREATE_VERTEX_BUFFER(name) vertex_buffer name(void*, uint32, uint8, ShaderDataType, ...)
typedef ENGINE_RENDER_CREATE_VERTEX_BUFFER(Engine_Render_CreateVertexBuffer_t);

#define ENGINE_RENDER_CREATE_INDEX_BUFFER(name) index_buffer name(uint32*, uint32)
typedef ENGINE_RENDER_CREATE_INDEX_BUFFER(Engine_Render_CreateIndexBuffer_t);

#define ENGINE_RENDER_CREATE_VERTEX_ARRAY(name) vertex_array_object name(vertex_buffer*, index_buffer*)
typedef ENGINE_RENDER_CREATE_VERTEX_ARRAY(Engine_Render_CreateVertexArray_t);

#define ENGINE_RENDER_DRAW_DEBUG_TEXT(name) void name(char* Text, real32 X, real32 Y, rh::laml::Vec3 Color, TextAlignment Alignment);
typedef ENGINE_RENDER_DRAW_DEBUG_TEXT(Engine_Render_DrawDebugText_t);


// Logging
#define ENGINE_DEBUG_LOG_MESSAGE(name) void name(const char* msg)
typedef ENGINE_DEBUG_LOG_MESSAGE(Engine_Debug_LogMessage_t);

// default
#define STUB_FUNCTION(name) void name(...)
typedef STUB_FUNCTION(EngineStubFunction_t);
STUB_FUNCTION(EngineStubFunction) {
    return;
}

struct gameImport_t {
    int Version;

    Engine_LoadDynamicFont_t *LoadDynamicFont;

    struct {
        Engine_Render_LoadShaderFromFile_t *LoadShaderFromFile;

        Engine_Render_CreateVertexBuffer_t *CreateVertexBuffer;
        Engine_Render_CreateIndexBuffer_t  *CreateIndexBuffer;
        Engine_Render_CreateVertexArray_t  *CreateVertexArray;

        Engine_Render_DrawDebugText_t *DrawDebugText;
    } Render;

    struct {
        Engine_Debug_LogMessage_t* LogMessage;
        Engine_Debug_LogMessage_t* LogError;
    } Logger;
};

#endif