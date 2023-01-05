#ifndef ENGINE_H
#define ENGINE_H

#include "Engine/Core/MemoryArena.hpp"
#include "Engine/Renderer/CommandBuffer.h"

/* Services that the Engine provides to the Game: */

// Logging
#define ENGINE_LOG_MESSAGE(name) void name(const char* msg)
typedef ENGINE_LOG_MESSAGE(EngineLogMessage_t);

// Filesystem
#define ENGINE_GET_EXE_PATH(name) const char* name(void)
typedef ENGINE_GET_EXE_PATH(EngineGetEXEPath_t);

// Renderer
#define ENGINE_RENDER_BEGIN_FRAME(name) void name(void)
typedef ENGINE_RENDER_BEGIN_FRAME(EngineRenderBeginFrame_t);

#define ENGINE_RENDER_END_FRAME(name) void name(void)
typedef ENGINE_RENDER_END_FRAME(EngineRenderEndFrame_t);

struct gameImport_t {
    int Version;

    // Logging stuff
    EngineLogMessage_t* LogMessage;
    
    // Filesystem stuff
    EngineGetEXEPath_t* GetEXEPath;

    // Renderer stuff
    EngineRenderBeginFrame_t* BeginFrame;
    EngineRenderEndFrame_t*   EndFrame;
};

#endif