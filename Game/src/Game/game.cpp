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

// Text rendering
struct shader_text {
    uint32 Handle;

    ShaderUniform_Vec4 r_transform;
    ShaderUniform_Vec4 r_transformUV;
    ShaderUniform_Mat4 r_orthoProjection;

    ShaderUniform_Sampler2D r_fontTex;
    ShaderUniform_Vec3 r_textColor;
};

struct text_renderer {
    shader_text Shader;

    vertex_array_object TextQuad;
    rh::laml::Mat4 orthoMat;
    dynamic_font Font;
};

struct game_state {
    real32 XValue;
    real32 YValue;

    real32 TValue;

    // Text rendering
    text_renderer TextRenderer;

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

    const char* ExePath; //tmp

    memory_arena FrameArena;
    memory_arena PrevFrameArena;
    memory_arena PermArena;
};

void SubmitText(dynamic_font* Font, const char Text[], real32 StartX, real32 StartY, rh::laml::Vec3 Color, TextAlignment Align) {

}

#define ezLoadShader(ShaderVar, ShaderPath) Engine.LoadShader((Shader*)(&ShaderVar), ShaderPath)

// TODO: Rewrite the game code to fit into these functions,
//       or add/remove these functions to better fit the game
GAME_INIT_FUNC(GameInit) {
    game_state* GameState = (game_state*)Memory->PermanentStorage;

    if (!Memory->IsInitialized) {
        GameState->ExePath = Engine.GetEXEPath();

        GameState->XValue = 0.5f;
        GameState->YValue = 0.5f;
        GameState->TValue = 0.0f;

        //BIND_UNIFORM(GameState->lineShader, r_test);

        // Init TextRenderer
        ezLoadShader(GameState->TextRenderer.Shader, "Data/Shaders/Text.glsl");
        GameState->TextRenderer.Shader.r_transform.Handle = 1;
        GameState->TextRenderer.Shader.r_transformUV.Handle = 2;
        GameState->TextRenderer.Shader.r_orthoProjection.Handle = 3;
        GameState->TextRenderer.Shader.r_fontTex.Handle = 4;
        GameState->TextRenderer.Shader.r_textColor.Handle = 5;

        GameState->TextRenderer.Shader.r_transform.value = {1.0f, 1.0f, 0.0f, 0.0f};
        GameState->TextRenderer.Shader.r_transformUV.value = {1.0f, 1.0f, 0.0f, 0.0f};

        rh::laml::transform::create_projection_orthographic(GameState->TextRenderer.orthoMat, 0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);
        real32 QuadVerts[] = {
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f
        };
        uint32 QuadIndices[] = {
            0, 1, 2,
            0, 2, 3
        };
        vertex_buffer VBO = Engine.CreateVertexBuffer(QuadVerts, sizeof(QuadVerts));
        Engine.SetVertexBufferLayout(&VBO, 1, ShaderDataType::Float2);
        index_buffer IBO = Engine.CreateIndexBuffer(QuadIndices, 6);
        GameState->TextRenderer.TextQuad = Engine.CreateVertexArray(&VBO, &IBO);

        Engine.LoadDynamicFont(&GameState->TextRenderer.Font, "Data/Fonts/UbuntuMono-Regular.ttf", 32, 512);

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

#define Render_ClearColor(CommandBuffer, r, g, b, a) \
    CMD_Clear_Buffer* VARNAME(cmd_clear_) = PushRenderCommand(CmdBuffer, CMD_Clear_Buffer); \
    VARNAME(cmd_clear_)->ClearColor.x = r;\
    VARNAME(cmd_clear_)->ClearColor.y = g;\
    VARNAME(cmd_clear_)->ClearColor.z = b;\
    VARNAME(cmd_clear_)->ClearColor.w = a;

    Render_ClearColor(CmdBuffer, GameState->XValue, GameState->YValue, 0.25f*sinf(1.0f*GameState->TValue) + 0.5f, 1.0f);

    // Render text
    {
        CMD_Bind_Shader* BindShaderCommand = PushRenderCommand(CmdBuffer, CMD_Bind_Shader);
        BindShaderCommand->ShaderHandle = GameState->TextRenderer.Shader.Handle;
        CMD_Bind_VAO* BindQuadCommand = PushRenderCommand(CmdBuffer, CMD_Bind_VAO);
        BindQuadCommand->VAOHandle = GameState->TextRenderer.TextQuad.Handle;
        CMD_Bind_Texture* BindTexCommand = PushRenderCommand(CmdBuffer, CMD_Bind_Texture);
        BindTexCommand->TextureSlot = 0;
        BindTexCommand->TextureHandle = GameState->TextRenderer.Font.TextureHandle;
        CMD_Set_Cull* SetCullCommand = PushRenderCommand(CmdBuffer, CMD_Set_Cull);
        SetCullCommand->Front = true;
        CMD_Set_Depth_Test* SetDepthCommand = PushRenderCommand(CmdBuffer, CMD_Set_Depth_Test);
        SetDepthCommand->Enabled = false;
        CMD_Upload_Uniform_int* TexIDCommand = PushRenderCommand(CmdBuffer, CMD_Upload_Uniform_int);
        TexIDCommand->Location = GameState->TextRenderer.Shader.r_fontTex.Handle;
        TexIDCommand->Value = 0;

        // Function call:
        char* Text = "Eggwuh";
        real32 StartX = 0.0f;
        real32 StartY = 0.0f;
        TextAlignment Alignment = TextAlignment::ALIGN_TOP_LEFT;
        rh::laml::Vec3 Color = { 1.0f, 1.0f, 1.0f };
        dynamic_font* Font = &GameState->TextRenderer.Font;

        {
            real32 X = StartX;
            real32 Y = StartY;

            real32 HOffset, VOffset;
            Engine.GetTextOffset(&GameState->TextRenderer.Font, &HOffset, &VOffset, Alignment, Text);

            CMD_Upload_Uniform_vec3* TextColorCmd = PushRenderCommand(CmdBuffer, CMD_Upload_Uniform_vec3);
            TextColorCmd->Location = GameState->TextRenderer.Shader.r_textColor.Handle;
            TextColorCmd->Value = Color;
            CMD_Upload_Uniform_mat4* ProjMatCmd = PushRenderCommand(CmdBuffer, CMD_Upload_Uniform_mat4);
            ProjMatCmd->Location = GameState->TextRenderer.Shader.r_orthoProjection.Handle;
            ProjMatCmd->Value = GameState->TextRenderer.orthoMat;

            while (*Text) {
                if (*Text == '\n') {
                    //Increase y by one line,
                    //reset x to start
                    X = StartX;
                    Y += GameState->TextRenderer.Font.FontSize;
                }
                if (*Text >= 32 && *Text < 128) {
                    stbtt_aligned_quad q;
                    char c = *Text - 32;
                    stbtt_GetBakedQuad(reinterpret_cast<stbtt_bakedchar*>(Font->cdata), Font->BitmapRes, Font->BitmapRes, *Text - 32, &X, &Y, &q, 1);//1=opengl & d3d10+,0=d3d9

                    float scaleX = q.x1 - q.x0;
                    float scaleY = q.y1 - q.y0;
                    float transX = q.x0;
                    float transY = q.y0;
                    CMD_Upload_Uniform_vec4* TransformCmd = PushRenderCommand(CmdBuffer, CMD_Upload_Uniform_vec4);
                    TransformCmd->Location = GameState->TextRenderer.Shader.r_transform.Handle;
                    TransformCmd->Value = rh::laml::Vec4(scaleX, scaleY, transX + HOffset, transY + VOffset);

                    scaleX = q.s1 - q.s0;
                    scaleY = q.t1 - q.t0;
                    transX = q.s0;
                    transY = q.t0;
                    CMD_Upload_Uniform_vec4* TransformUVCmd = PushRenderCommand(CmdBuffer, CMD_Upload_Uniform_vec4);
                    TransformUVCmd->Location = GameState->TextRenderer.Shader.r_transformUV.Handle;
                    TransformUVCmd->Value = rh::laml::Vec4(scaleX, scaleY, transX, transY);

                    CMD_Submit* SubmitCommand = PushRenderCommand(CmdBuffer, CMD_Submit);
                    SubmitCommand->IndexCount = GameState->TextRenderer.TextQuad.IndexCount;
                }
                ++Text;
            }
        }

        SetDepthCommand = PushRenderCommand(CmdBuffer, CMD_Set_Depth_Test);
        SetDepthCommand->Enabled = true;
        SetCullCommand = PushRenderCommand(CmdBuffer, CMD_Set_Cull);
        SetCullCommand->Front = false;
    }

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

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"