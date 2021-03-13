#pragma once

#include "RenderCommand.hpp"
#include "Shader.hpp"

namespace Engine {
    class TextRenderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene();
        static void EndScene();
        static void Flush();

        static void SubmitText(const std::string& text, float startX, float startY, float size, math::vec3 color);

        static void OnWindowResize(uint32_t width, uint32_t height);

        static const std::unique_ptr<ShaderLibrary>& GetShaderLibrary();
    };

}