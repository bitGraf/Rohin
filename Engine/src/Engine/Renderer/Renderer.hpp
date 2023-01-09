#if 1

#ifndef RENDERER_H
#define RENDERER_H

#include <stb_truetype.h>

#include "Engine/Renderer/CommandBuffer.h"
#include "Engine/Renderer/Light.hpp"

// Camera
enum camera_type {
    Perspective = 0,
    Orthographic
};
struct camera_perspective {
    real32 m_PerspectiveVerticalFoV;
    real32 m_PerspectiveNear;
    real32 m_PerspectiveFar;
};
struct camera_orthographic {
    real32 m_OrthographicSize;
    real32 m_OrthographicNear; 
    real32 m_OrthographicFar;
};
struct scene_camera {
    float m_AspectRatio;
    camera_type Type;

    union {
        camera_perspective Perspective;
        camera_perspective Orthographic;
    };
};

// Shader
struct shader {
    uint32 Handle;
};

struct pbr_parameters {
    float Roughness;
    float Metalness;

    rh::laml::Vec3 Normal;
    rh::laml::Vec3 View;
    float NdotV;

    rh::laml::Vec3 Diffuse;
    rh::laml::Vec3 Specular;
};

enum class ShaderDataType : uint8 {
    None = 0, 
    Float, Float2, Float3, Float4,
    Mat3, Mat4, 
    Int, Int2, Int3, Int4,
    Bool
};

#define MAX_ATTRIBUTES 8
struct vertex_buffer {
    uint32 Handle;

    uint8 NumAttributes;
    ShaderDataType Attributes[MAX_ATTRIBUTES];
};

struct index_buffer {
    uint32 Handle;
    uint32 IndexCount;
};

struct vertex_array_object { 
    uint32 Handle; 
    uint32 IndexCount;
};


/* Text alignmnet */
enum class TextAlignment : uint8 {
    ALIGN_TOP_LEFT = 0,
    ALIGN_MID_LEFT,
    ALIGN_BOT_LEFT,
    ALIGN_TOP_MID,
    ALIGN_MID_MID,
    ALIGN_BOT_MID,
    ALIGN_TOP_RIGHT,
    ALIGN_MID_RIGHT,
    ALIGN_BOT_RIGHT
};
struct dynamic_font {
    bool32 Initialized;
    stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
    uint32 TextureHandle;
    real32 FontSize;
    uint32 BitmapRes;
};

void GetTextOffset(dynamic_font* Font, real32 *HOffset, real32 *VOffset, TextAlignment Alignment, const char* Text);

/* * * * * * * * * * * * * * * * * * * * * *
Macro to define uniforms of the prototype:

    struct ShaderUniform_##Name { 
        uint32 Handle; 
        Typename value; 
    };
*/
#define SHADER_UNIFORM_TYPE_DECL(Name, Typename) struct ShaderUniform_##Name { uint32 Location; /*Typename value;*/ };

SHADER_UNIFORM_TYPE_DECL(int, int);
SHADER_UNIFORM_TYPE_DECL(float, float);
SHADER_UNIFORM_TYPE_DECL(vec2, rh::laml::Vec2);
SHADER_UNIFORM_TYPE_DECL(vec3, rh::laml::Vec3);
SHADER_UNIFORM_TYPE_DECL(vec4, rh::laml::Vec4);
SHADER_UNIFORM_TYPE_DECL(mat4, rh::laml::Mat4);
SHADER_UNIFORM_TYPE_DECL(sampler2D, int);
SHADER_UNIFORM_TYPE_DECL(samplerCube, int);
struct ShaderUniform_Light {
    ShaderUniform_vec3 Position;
    ShaderUniform_vec3 Direction;
    ShaderUniform_vec3 Color;
    ShaderUniform_float Strength;
    ShaderUniform_float Inner;
    ShaderUniform_float Outer;
};

// Text rendering
// TODO: Split shader folder into game/engine folder
struct shader_Text {
    uint32 Handle;

    ShaderUniform_vec4 r_transform;
    ShaderUniform_vec4 r_transformUV;
    ShaderUniform_mat4 r_orthoProjection;
    ShaderUniform_sampler2D r_fontTex;
    ShaderUniform_vec3 r_textColor;

    void InitShaderLocs() {
        r_transform.Location = 1;
        r_transformUV.Location = 2;
        r_orthoProjection.Location = 3;
        r_fontTex.Location = 4;
        r_textColor.Location = 5;
    }
};

struct text_renderer {
    shader_Text Shader;

    vertex_array_object TextQuad;
    rh::laml::Mat4 orthoMat;
    dynamic_font Font;
};

struct debug_render_state {
    text_renderer DebugTextRenderer;
};

#endif




#else
#pragma once

#include "RenderCommand.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Light.hpp"

//#include "TextRenderer.hpp"
#include "Engine/GameObject/Components.hpp"

namespace rh {
    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        /*  Anatomy of a frame

        - Once lighting and camera data has been determined:
        Begin3DScene(camera, lights)

            BeginDeferedPrepass();
                - Render all 3d meshes to fill the G-Buffer
            EndDeferedPrepass(); - perform lighting pipeline on G-Buffer

            RenderSkybox();

            - Prep a buffer to render to
            BeginSobel()
                - Render meshes normally
            - Perform sobel operator on the buffer
            EndSobel()

        - End scene, flush to screen
        End3DScene()

        RenderDebugUI();
        */

        // Set all shader constants and save for later
        static void Begin3DScene(const Camera& camera, const laml::Mat4& transform, 
            u32 numPointLights, const Light pointLights[32],
            u32 numSpotLights,  const Light spotLights[32],
            const Light& sun);
        static void End3DScene();

        // start writing to pre-pass buffers
        static void BeginDeferredPrepass();
        // End pre-pass, and perform lighting stages
        static void EndDeferredPrepass();

        // Prepare buffer to be filled with color
        static void BeginSobelPass();
        // Perform sobel operator on
        static void EndSobelPass();

        // Render text and other UI to the screen
        static void RenderDebugUI();

        // end pre-pass and begin render pipeline
        static void UpdateLighting(const laml::Mat4& ViewMatrix,
            u32 numPointLights, const Light pointLights[32],
            u32 numSpotLights, const Light spotLights[32],
            const Light& sun, const laml::Mat4& projection);
        static void UploadLights(const Ref<shader> shader);
        static void Flush();
        static void Precompute();

        static void NextOutputMode();
        static void PrintState();
        static void ToggleToneMapping();
        static void ToggleGammaCorrection();
        static void ToggleDebugSoundOutput();
        static void ToggleDebugControllerOutput();

        static void Submit(const laml::Mat4& transform = laml::Mat4(1.0f));
        static void Submit(const Ref<VertexArray>& vao, const laml::Mat4& transform, const laml::Vec3& color);
        static void SubmitMesh(const Mesh* mesh, const laml::Mat4& transform);
        // ANIM_HOOK static void SubmitMesh(const Mesh* mesh, const laml::Mat4& transform, md5::Animation* anim); // For animation
        static void SubmitMesh_drawNormals(const Ref<Mesh>& mesh, const laml::Mat4& transform);

        static void Draw3DText(const std::string& text, const laml::Vec3& pos, const laml::Vec3& color);
        static void DrawSkeletonDebug(
            const TagComponent& tag, 
            const TransformComponent& transform, 
            const MeshRendererComponent& mesh, 
            const laml::Vec3& text_color,
            bool bind_pose = false);

        static void SubmitLine(const laml::Vec3& v0, const laml::Vec3& v1, const laml::Vec4& color);

        static void SubmitFullscreenQuad();

        static void RecompileShaders();
        static void OnWindowResize(uint32_t width, uint32_t height);

        static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

        static const std::unique_ptr<ShaderLibrary>& GetShaderLibrary();
    };

}
#endif