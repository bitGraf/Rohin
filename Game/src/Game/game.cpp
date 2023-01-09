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
#include "Engine/Resources/triangle_mesh.cpp"

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

    // Meshes
    uint32 NumMeshes;
    triangle_mesh* TriangleMeshes;

    // PermanentArenas
    memory_arena FrameArena;
    memory_arena PrevFrameArena;
    memory_arena PermArena;
    // TransientArenas
    memory_arena MeshArena;
    memory_arena TransArena;
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
                        Memory->PermanentStorageSize - sizeof(game_state) - Megabytes(4), 
                        ((uint8*)Memory->PermanentStorage + sizeof(game_state) + Megabytes(4)));

        Engine.Logger.LogMessage("Done with initialization! %d\n", 5);

        // Transient initialization
        InitializeArena(&GameState->MeshArena,
                        Megabytes(64),
                        (uint8*)Memory->TransientStorage);
        InitializeArena(&GameState->TransArena,
                        Memory->TransientStorageSize - Megabytes(64),
                        (uint8*)Memory->TransientStorage + Megabytes(64));

        GameState->NumMeshes = 1;
        GameState->TriangleMeshes = PushArray(&GameState->MeshArena, triangle_mesh, GameState->NumMeshes);
        //Engine.LoadMeshFromFile(&GameState->TransArena, &GameState->TriangleMeshes[0], "Data/Models/dance.mesh");
        uint32 buffSize;
        uint8* buffer = Engine.ReadEntireFile(&GameState->TransArena, "Data/Models/dance.mesh", &buffSize);
        LoadMeshFromBuffer(&GameState->TriangleMeshes[0], buffer, buffSize);

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

    // render each mesh
    for (uint32 n = 0; n < GameState->NumMeshes; n++) {
        triangle_mesh* Mesh = &GameState->TriangleMeshes[n];

        //Render_BindVAO(CmdBuffer, Mesh->VertexArray);
        //Render_Submit(CmdBuffer, Mesh->VertexArray.Handle);
    }

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