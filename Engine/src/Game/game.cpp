#include "game.h"

#include "Engine/Core/MemoryArena.cpp"

static gameImport_t *Engine;

struct game_state {
    int32 value;

    const char* ExePath; //tmp

    memory_arena CurrentScene;
};

// TODO: Rewrite the game code to fit into these functions,
//       or add/remove these functions to better fit the game
GAME_INIT_FUNC(GameInit) {
    game_state* GameState = (game_state*)Memory->PermanentStorage;

    if (!Memory->IsInitialized) {
        GameState->value = 0;
        GameState->ExePath = Engine->GetEXEPath();

        InitializeArena(&GameState->CurrentScene, Kilobytes(1), (uint8*)Memory->TransientStorage);

        Memory->IsInitialized = true;
    }
}
GAME_FRAME_FUNC(GameFrame) {
    game_state* GameState = (game_state*)Memory->PermanentStorage;

    if (Input->MouseButtons[0].EndedDown) {
        GameState->value++;
    }

    if (GameState->value > 5) {
        Assert(false);
    }
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

    if (Import && Import->Version == Export.Version) {
        Engine = Import;

        //Engine->GetEXEPath();
    } 

    return Export;
}