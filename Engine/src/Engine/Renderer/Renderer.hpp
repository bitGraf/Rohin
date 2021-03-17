#pragma once

#include "RenderCommand.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Light.hpp"

//#include "TextRenderer.hpp"

namespace Engine {
    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        // start writing to pre-pass buffers
        static void Begin3DScene(const Camera& camera, const math::mat4& transform, 
            u32 numPointLights, const Light pointLights[32],
            u32 numSpotLights,  const Light spotLights[32],
            const Light& sun);

        // end pre-pass and begin render pipeline
        static void End3DScene();
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

        static void Submit(const math::mat4& transform = math::mat4());
        static void Submit(const Camera& camera, const math::mat4& camTrans, const Ref<VertexArray>& vao, const math::mat4& transform, const math::vec3& color);
        static void SubmitMesh(const Ref<Mesh>& mesh, const math::mat4& transform);
        static void SubmitMesh_drawNormals(const Ref<Mesh>& mesh, const math::mat4& transform);

        static void SubmitFullscreenQuad();

        static void OnWindowResize(uint32_t width, uint32_t height);

        static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

        static const std::unique_ptr<ShaderLibrary>& GetShaderLibrary();
    };

}