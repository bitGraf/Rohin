#include "game.h"

#include "Engine/Core/MemoryArena.cpp"
//#include "Engine/Scene/Scene3D.cpp"
#include "Engine/Renderer/CommandBuffer.cpp"

static gameImport_t Engine;

struct game_state {
    int32 value;

    const char* ExePath; //tmp

    memory_arena RenderArena;
};

// TODO: Rewrite the game code to fit into these functions,
//       or add/remove these functions to better fit the game
GAME_INIT_FUNC(GameInit) {
    game_state* GameState = (game_state*)Memory->PermanentStorage;

    if (!Memory->IsInitialized) {
        GameState->value = 0;
        GameState->ExePath = Engine.GetEXEPath();

        InitializeArena(&GameState->RenderArena, Memory->TransientStorageSize, (uint8*)Memory->TransientStorage);

        Memory->IsInitialized = true;
    }
}

GAME_FRAME_FUNC(GameFrame) {
    game_state* GameState = (game_state*)Memory->PermanentStorage;

    // Handle input/update state
    if (Input->MouseButtons[0].EndedDown) {
        GameState->value++;
    }

    Engine.BeginFrame(); // <-- should this round-tripping be nececary?

    // Create the render command buffer <-- should probably be managed by the Engine, and passed in?
    //CmdBuffer = CreateRenderCommandBuffer(&GameState->RenderArena, Megabytes(2));

#if 0
#define ezRenderCommand(Command, handle) Command* handle = PushRenderCommand(CmdBuffer, Command)
#define ezRenderCommand_(Command) PushRenderCommand(CmdBuffer, Command)
#endif

    CMD_Clear_Buffer* cmd_clear = PushRenderCommand(CmdBuffer, CMD_Clear_Buffer);
    cmd_clear->ClearColor.x = 1.0f;
    cmd_clear->ClearColor.y = 1.0f;
    cmd_clear->ClearColor.z = 0.0f;
    cmd_clear->ClearColor.w = 1.0f;

#if 0
    CMD_Bind_Framebuffer* cmd_bind_fbo = PushRenderCommand(CmdBuffer, CMD_Bind_Framebuffer);
    cmd_bind_fbo->FramebufferId = 2;

    CMD_Bind_Texture* cmd_b1 = PushRenderCommand(CmdBuffer, CMD_Bind_Texture);
    cmd_b1->TextureId = 11;

    CMD_Bind_Texture* cmd_b2 = PushRenderCommand(CmdBuffer, CMD_Bind_Texture);
    cmd_b2->TextureId = 12;

    CMD_Bind_Texture* cmd_b3 = PushRenderCommand(CmdBuffer, CMD_Bind_Texture);
    cmd_b3->TextureId = 13;

    CMD_Bind_VAO* cmd_bind_vao = PushRenderCommand(CmdBuffer, CMD_Bind_VAO);
    cmd_bind_vao->VAOId = 5;

    CMD_Submit* cmd_submit = PushRenderCommand(CmdBuffer, CMD_Submit);
    cmd_submit->NumVerts = 55;
    cmd_submit->NumIndices = 124;
#endif

    Engine.EndFrame();
}

GAME_EVENT_FUNC(GameEvent) {
    game_state* GameState = (game_state*)Memory->PermanentStorage;
}

GAME_SHUTDOWN_FUNC(GameShutdown) {
    game_state* GameState = (game_state*)Memory->PermanentStorage;
    Memory->IsInitialized = false; // force a reinitializtion
}

// extern "C" gameExport_t* GetGameAPI(gameImport_t Import) {
extern "C" GAME_GET_API_FUNC(GetGameAPI) {
    gameExport_t Export = {};
    Export.Version = 1;

    Export.Init = &GameInit;
    Export.Frame = &GameFrame;
    Export.HandleEvent = &GameEvent;
    Export.Shutdown = &GameShutdown;

    if (Import.Version == Export.Version) {
        Engine = Import;

        //Engine->GetEXEPath();
    }

    return Export;
}