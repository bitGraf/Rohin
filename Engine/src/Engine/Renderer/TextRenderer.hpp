#pragma once

#include "RenderCommand.hpp"
#include "Shader.hpp"

namespace Engine {

    class TextRenderer
    {
    public:
        static void Init();
        static void Shutdown();

        //static void BeginTextRendering();
        //static void EndTextRendering();
        //static void Flush();

        static void SubmitText(const std::string& text, float startX, float startY, math::vec3 color, TextAlignment align = TextAlignment::ALIGN_TOP_LEFT);

        static void OnWindowResize(uint32_t width, uint32_t height);

        static const std::unique_ptr<ShaderLibrary>& GetShaderLibrary();
    };

}