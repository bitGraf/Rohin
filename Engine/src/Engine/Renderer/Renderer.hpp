#pragma once

#include "RenderCommand.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"

namespace Engine {
    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();


        static void BeginScene(const Camera& camera, const math::mat4& transform);
        static void RenderDebug(const Camera& camera, const math::mat4& transform);
        static void EndScene();
        static void Flush();

        static void Submit(const math::mat4& transform = math::mat4());
        static void SubmitMesh(const Ref<Mesh>& mesh, const math::mat4& transform);
        static void SubmitMesh_drawNormals(const Ref<Mesh>& mesh, const math::mat4& transform);

        static void SubmitFullscreenQuad();

        static void OnWindowResize(uint32_t width, uint32_t height);

        static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

        static const std::unique_ptr<ShaderLibrary>& GetShaderLibrary();
    };

}