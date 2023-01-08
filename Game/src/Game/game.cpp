#include "game.h"

//#include "Engine/Scene/Scene3D.cpp"

// temp -> these are being compiled in the Engine.exe 
// currently, but unless we pass those function pointers 
// in the GameImport struct, this .dll treats them as 
// unresulved externals...
// Something to figure out.
#include "Engine/Core/MemoryArena.cpp"
#include "Engine/Renderer/CommandBuffer.cpp"
#include "Engine/Renderer/Renderer.hpp"

// CRT
#include <cmath>

static gameImport_t Engine;

#include "ShaderSrc/shaders_generated.cpp"

struct game_state {
    real32 XValue;
    real32 YValue;

    real32 TValue;

    // Shaders
    shader_Line ShaderLine;
    shader_Line3D ShaderLine3D;
    shader_PrePass_Anim ShaderPrepassAnim;
    shader_PrePass ShaderPrepass;
    shader_SSAO ShaderSSAO;
    shader_Lighting ShaderLighting;
    shader_Sobel ShaderSobel;
    shader_Screen ShaderScreen;
    shader_Mix ShaderMix;

    memory_arena FrameArena;
    memory_arena PrevFrameArena;
    memory_arena PermArena;
};

#define ezLoadShader(ShaderVar, ShaderPath) \
    Engine.Render.LoadShaderFromFile((shader*)(&ShaderVar), ShaderPath);\
    ShaderVar.InitShaderLocs();

// TODO: Rewrite the game code to fit into these functions,
//       or add/remove these functions to better fit the game
GAME_INIT_FUNC(GameInit) {
    game_state* GameState = (game_state*)Memory->PermanentStorage;

    if (!Memory->IsInitialized) {
        GameState->XValue = 0.5f;
        GameState->YValue = 0.5f;
        GameState->TValue = 0.0f;

        //BIND_UNIFORM(GameState->lineShader, r_test);

        // Init 3D renderer
        ezLoadShader(GameState->ShaderLine, "Data/Shaders/Line.glsl");
        ezLoadShader(GameState->ShaderLine3D, "Data/Shaders/Line3D.glsl");
        ezLoadShader(GameState->ShaderPrepassAnim, "Data/Shaders/PrePass.glsl");
        ezLoadShader(GameState->ShaderPrepass, "Data/Shaders/PrePass_Anim.glsl");
        ezLoadShader(GameState->ShaderSSAO, "Data/Shaders/Lighting.glsl");
        ezLoadShader(GameState->ShaderLighting, "Data/Shaders/SSAO.glsl");
        ezLoadShader(GameState->ShaderSobel, "Data/Shaders/Screen.glsl");
        ezLoadShader(GameState->ShaderScreen, "Data/Shaders/Sobel.glsl");
        ezLoadShader(GameState->ShaderMix, "Data/Shaders/Mix.glsl");
        

        InitializeArena(&GameState->FrameArena, 
                        Megabytes(2), 
                        ((uint8*)Memory->PermanentStorage + sizeof(game_state)));
        InitializeArena(&GameState->PrevFrameArena, 
                        Megabytes(2), 
                        ((uint8*)Memory->PermanentStorage + sizeof(game_state) + Megabytes(2)));
        InitializeArena(&GameState->PermArena, 
                        Memory->PermanentStorageSize - sizeof(game_state), 
                        ((uint8*)Memory->PermanentStorage + sizeof(game_state) + Megabytes(4)));

        Engine.Logger.LogMessage("Done with initialization! %d\n", 5);

        Memory->IsInitialized = true;
    }
}

GAME_FRAME_FUNC(GameFrame) {
    game_state* GameState = (game_state*)Memory->PermanentStorage;
    // Clear the frame arena for a new frame
    ClearArena(&GameState->FrameArena);

    // Handle input/update state
    game_controller_input* Controller = GetController(Input, 0);
    if (Controller->IsConnected && Controller->IsAnalog) {
        GameState->XValue = Controller->StickAverageX;
        GameState->YValue = Controller->StickAverageY;
    } else {
        GameState->XValue = (Input->MouseX / 640.0f) - 1.0f;
        GameState->YValue = (Input->MouseY / 360.0f) - 1.0f;
    }

    GameState->TValue += Input->dtForFrame;

    // Render

    Render_ClearColor(CmdBuffer, GameState->XValue, GameState->YValue, 0.25f*sinf(1.0f*GameState->TValue) + 0.5f, 1.0f);

    // Render text
    //void name(char* Text, real32 X, real32 Y, rh::laml::Vec3 Color, TextAlignment Alignment);
    rh::laml::Vec3 TextColor(1.0f, 1.0f, 1.0f);
    Engine.Render.DrawDebugText("Eggwuh", 0.0f, 0.0f, 
                                TextColor, TextAlignment::ALIGN_TOP_LEFT);

    // Swap the two arenas, allowing the last frame to be saved
    SwapArenas(GameState->FrameArena, GameState->PrevFrameArena);
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

    if (Import.Version == Export.Version) {
        Engine = Import;

        Export.Init = &GameInit;
        Export.Frame = &GameFrame;
        Export.HandleEvent = &GameEvent;
        Export.Shutdown = &GameShutdown;
    } else {
        Export.Invalid = true;
    }

    return Export;
}