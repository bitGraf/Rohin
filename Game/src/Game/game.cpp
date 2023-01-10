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
#include "Game/entity.h"

// CRT
#include <cmath>

static gameImport_t Engine;

#include "Engine/Resources/triangle_mesh.cpp"
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
    shader_simple ShaderSimple;

    // Main Camera
    //scene_camera Camera;
    rh::laml::Vec3 CameraPosition;

    rh::laml::Mat4 CameraTransform;
    rh::laml::Mat4 ViewMatrix;

    rh::laml::Mat4 ProjectionMatrix;

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

        GameState->CameraPosition.x = 0.0f;
        GameState->CameraPosition.y = 2.0f;
        GameState->CameraPosition.z = 5.0f;
        //rh::laml::Mat4 rotM;
        rh::laml::transform::create_transform_translate(GameState->CameraTransform, GameState->CameraPosition);
        //rh::laml::transform::create_transform_rotation(rotM, 0.0f, -45.0f, 0.0f);
        //GameState->CameraTransform = rh::laml::mul(GameState->CameraTransform, rotM);
        rh::laml::transform::create_view_matrix_from_transform(GameState->ViewMatrix, GameState->CameraTransform);

        rh::laml::transform::create_projection_perspective(GameState->ProjectionMatrix, 75.0f, 16.0f/9.0f, 0.1f, 20.0f);

        entity_registry Registry = {};
        EntityTest(&Registry);

        //BIND_UNIFORM(GameState->lineShader, r_test);

        // Init 3D renderer
        ezLoadShader(GameState->ShaderLine,        "Data/Shaders/Line.glsl");
        ezLoadShader(GameState->ShaderLine3D,      "Data/Shaders/Line3D.glsl");
        ezLoadShader(GameState->ShaderPrepassAnim, "Data/Shaders/PrePass.glsl");
        ezLoadShader(GameState->ShaderPrepass,     "Data/Shaders/PrePass_Anim.glsl");
        ezLoadShader(GameState->ShaderSSAO,        "Data/Shaders/Lighting.glsl");
        ezLoadShader(GameState->ShaderLighting,    "Data/Shaders/SSAO.glsl");
        ezLoadShader(GameState->ShaderSobel,       "Data/Shaders/Screen.glsl");
        ezLoadShader(GameState->ShaderScreen,      "Data/Shaders/Sobel.glsl");
        ezLoadShader(GameState->ShaderMix,         "Data/Shaders/Mix.glsl");
        ezLoadShader(GameState->ShaderSimple,      "Data/Shaders/simple.glsl");

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
        LoadMeshFromBuffer(&GameState->MeshArena, &GameState->TriangleMeshes[0], buffer, buffSize);
        GameState->TransArena.Used -= buffSize;

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
    Render_BindShader(CmdBuffer, GameState->ShaderSimple);
    Render_UploadVec4(CmdBuffer, GameState->ShaderSimple.u_color, rh::laml::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Render_UploadMat4(CmdBuffer, GameState->ShaderSimple.r_VP, rh::laml::mul(GameState->ProjectionMatrix, GameState->ViewMatrix));
    Render_UploadMat4(CmdBuffer, GameState->ShaderSimple.r_Transform, rh::laml::Mat4(1.0f, 1.0f, 1.0f, 1.0f));
    for (uint32 n = 0; n < GameState->NumMeshes; n++) {
        triangle_mesh* Mesh = &GameState->TriangleMeshes[n];

        Render_BindVAO(CmdBuffer, Mesh->VertexArray);
        Render_Submit(CmdBuffer, Mesh->VertexArray.IndexCount);
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