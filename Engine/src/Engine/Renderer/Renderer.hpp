#pragma once

#include "RenderCommand.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Light.hpp"

//#include "TextRenderer.hpp"
#include "Engine/GameObject/Components.hpp"

namespace Engine {
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
        static void Begin3DScene(const Camera& camera, const math::mat4& transform, 
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
        static void UpdateLighting(const math::mat4& ViewMatrix,
            u32 numPointLights, const Light pointLights[32],
            u32 numSpotLights, const Light spotLights[32],
            const Light& sun, const math::mat4& projection);
        static void UploadLights(const Ref<Shader> shader);
        static void Flush();
        static void Precompute();

        static void NextOutputMode();
        static void PrintState();
        static void ToggleToneMapping();
        static void ToggleGammaCorrection();
        static void ToggleDebugSoundOutput();
        static void ToggleDebugControllerOutput();

        static void Submit(const math::mat4& transform = math::mat4());
        static void Submit(const Ref<VertexArray>& vao, const math::mat4& transform, const math::vec3& color);
        static void SubmitMesh(const Mesh* mesh, const math::mat4& transform);
        static void SubmitMesh(const Mesh* mesh, const math::mat4& transform, md5::Animation* anim); // For animation
        static void SubmitMesh_drawNormals(const Ref<Mesh>& mesh, const math::mat4& transform);

        static void Draw3DText(const std::string& text, const math::vec3& pos, const math::vec3 color);
        static void DrawSkeletonDebug(
            const TagComponent& tag, 
            const TransformComponent& transform, 
            const MeshRendererComponent& mesh, 
            const MeshAnimationComponent& anim,
            const math::vec3 color);

        static void SubmitLine(math::vec3 v0, math::vec3 v1, math::vec4 color);

        static void SubmitFullscreenQuad();

        static void RecompileShaders();
        static void OnWindowResize(uint32_t width, uint32_t height);

        static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

        static const std::unique_ptr<ShaderLibrary>& GetShaderLibrary();
    };

}