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

struct shader_line {
    uint32 Handle;

    ShaderUniform_Mat4 r_VP;
    ShaderUniform_Mat4 r_Transform;

    ShaderUniform_Vec3 r_CamPos;
    ShaderUniform_Vec3 r_LineColor;
    ShaderUniform_Float r_LineFadeStart;
    ShaderUniform_Float r_LineFadeEnd;
    ShaderUniform_Float r_LineFadeMaximum;
    ShaderUniform_Float r_LineFadeMinumum;
};

struct shader_line_3D {
    uint32 Handle; // <-- this is needed so that (shader*)shaderLine3D will be valid, and Handle will be correct in both cases

    ShaderUniform_Vec3 r_verts[2];
    ShaderUniform_Mat4 r_View;
    ShaderUniform_Mat4 r_Projection;

    ShaderUniform_Vec4 r_Color;
};

struct shader_prepass_anim {
    uint32 Handle;

    ShaderUniform_Mat4 r_Bones[128];
    ShaderUniform_Mat4 r_Transform;
    ShaderUniform_Mat4 r_View;
    ShaderUniform_Mat4 r_Projection;

    ShaderUniform_Sampler2D u_AlbedoTexture;
    ShaderUniform_Sampler2D u_NormalTexture;
    ShaderUniform_Sampler2D u_MetalnessTexture;
    ShaderUniform_Sampler2D u_RoughnessTexture;
    ShaderUniform_Sampler2D u_AmbientTexture;
    ShaderUniform_Sampler2D u_EmissiveTexture;
    ShaderUniform_Vec3 u_AlbedoColor;
    ShaderUniform_Float u_Metalness;
    ShaderUniform_Float u_Roughness;
    ShaderUniform_Float u_TextureScale;
    ShaderUniform_Float r_AlbedoTexToggle;
    ShaderUniform_Float r_NormalTexToggle;
    ShaderUniform_Float r_MetalnessTexToggle;
    ShaderUniform_Float r_RoughnessTexToggle;
    ShaderUniform_Float r_AmbientTexToggle;
    ShaderUniform_Float r_EmissiveTexToggle;
    ShaderUniform_Float r_gammaCorrect;
};

struct shader_prepass {
    uint32 Handle;

    ShaderUniform_Mat4 r_Transform;
    ShaderUniform_Mat4 r_View;
    ShaderUniform_Mat4 r_Projection;

    ShaderUniform_Sampler2D u_AlbedoTexture;
    ShaderUniform_Sampler2D u_NormalTexture;
    ShaderUniform_Sampler2D u_MetalnessTexture;
    ShaderUniform_Sampler2D u_RoughnessTexture;
    ShaderUniform_Sampler2D u_AmbientTexture;
    ShaderUniform_Sampler2D u_EmissiveTexture;
    ShaderUniform_Vec3 u_AlbedoColor;
    ShaderUniform_Float u_Metalness;
    ShaderUniform_Float u_Roughness;
    ShaderUniform_Float u_TextureScale;
    ShaderUniform_Float r_AlbedoTexToggle;
    ShaderUniform_Float r_NormalTexToggle;
    ShaderUniform_Float r_MetalnessTexToggle;
    ShaderUniform_Float r_RoughnessTexToggle;
    ShaderUniform_Float r_AmbientTexToggle;
    ShaderUniform_Float r_EmissiveTexToggle;
    ShaderUniform_Float r_gammaCorrect;
};

struct shader_ssao {
    uint32 Handle;

    // no vertex uniforms

    ShaderUniform_Sampler2D u_amr;
};

struct shader_Lighting {
    uint32 Handle;

    ShaderUniform_Mat4 r_Projection;

    ShaderUniform_Light r_pointLights[32];
    ShaderUniform_Light r_spotLights[32];
    ShaderUniform_Light r_sun[32];
    ShaderUniform_Mat4 r_View;
    ShaderUniform_Sampler2D u_normal;
    ShaderUniform_Sampler2D u_distance;
    ShaderUniform_Sampler2D u_amr;
    ShaderUniform_PBRParameters m_Params;
};

struct shader_Sobel {
    uint32 Handle;

    // no vertex uniforms

    ShaderUniform_Sampler2D r_texture;
};

struct shader_Screen {
    uint32 Handle;

    // no vertex uniforms

    ShaderUniform_Sampler2D u_albedo;
    ShaderUniform_Sampler2D u_normal;
    ShaderUniform_Sampler2D u_amr;
    ShaderUniform_Sampler2D u_depth;
    ShaderUniform_Sampler2D u_diffuse;
    ShaderUniform_Sampler2D u_specular;
    ShaderUniform_Sampler2D u_emissive;
    ShaderUniform_Sampler2D u_ssao;
    ShaderUniform_Int r_outputSwitch;
    ShaderUniform_Float r_toneMap;
    ShaderUniform_Float r_gammaCorrect;
};

struct shader_Mix {
    uint32 Handle;

    // no vertex uniforms

    ShaderUniform_Sampler2D r_tex1;
    ShaderUniform_Sampler2D r_tex2;
    ShaderUniform_Float r_mixValue;
};

struct game_state {
    real32 XValue;
    real32 YValue;

    real32 TValue;

    // Shaders
    shader_line ShaderLine;
    shader_line_3D ShaderLine3D;
    shader_prepass_anim ShaderPrepassAnim;
    shader_prepass ShaderPrepass;
    shader_ssao ShaderSSAO;
    shader_Lighting ShaderLighting;
    shader_Sobel ShaderSobel;
    shader_Screen ShaderScreen;
    shader_Mix ShaderMix;

    memory_arena FrameArena;
    memory_arena PrevFrameArena;
    memory_arena PermArena;
};

void SubmitText(dynamic_font* Font, const char Text[], real32 StartX, real32 StartY, rh::laml::Vec3 Color, TextAlignment Align) {

}

#define ezLoadShader(ShaderVar, ShaderPath) Engine.Render.LoadShaderFromFile((Shader*)(&ShaderVar), ShaderPath)

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
        //GameState->ShaderLine.
        ezLoadShader(GameState->ShaderLine3D, "Data/Shaders/Line3D.glsl");
        GameState->ShaderLine3D.r_verts[0].Handle = 1;
        GameState->ShaderLine3D.r_verts[1].Handle = 2;
        GameState->ShaderLine3D.r_View.Handle = 3;
        GameState->ShaderLine3D.r_Projection.Handle = 4;
        GameState->ShaderLine3D.r_Color.Handle = 5;
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

    // macro test, dont really like it >>
#define CAT_(a, b) a ## b
#define CAT(a, b) CAT_(a, b)
#define VARNAME(Var) CAT(Var, __LINE__)

#define Render_ClearColor(GlobalCommandBuffer, r, g, b, a) \
    CMD_Clear_Buffer* VARNAME(cmd_clear_) = PushRenderCommand(CmdBuffer, CMD_Clear_Buffer); \
    VARNAME(cmd_clear_)->ClearColor.x = r;\
    VARNAME(cmd_clear_)->ClearColor.y = g;\
    VARNAME(cmd_clear_)->ClearColor.z = b;\
    VARNAME(cmd_clear_)->ClearColor.w = a;

    Render_ClearColor(CmdBuffer, GameState->XValue, GameState->YValue, 0.25f*sinf(1.0f*GameState->TValue) + 0.5f, 1.0f);

    // Render text
    //void name(char* Text, real32 X, real32 Y, rh::laml::Vec3 Color, TextAlignment Alignment);
    rh::laml::Vec3 TextColor(1.0f, 1.0f, 1.0f);
    real32 y = 0.0f;
    Engine.Render.DrawDebugText("Eggwuh", 0.0f, y, 
                                TextColor, TextAlignment::ALIGN_TOP_LEFT);
    Engine.Render.DrawDebugText("Running...", 0.0f, y += 32.0f, 
                                TextColor, TextAlignment::ALIGN_TOP_LEFT);
    Engine.Render.DrawDebugText("hehe", 0.0f, y += 32.0f, 
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