#pragma once

#include "RenderCommand.hpp"
#include "Shader.hpp"

namespace Engine {

    struct SpriteRect {
        float x0, y0, x1=-1, y1=-1;
    };

    class SpriteRenderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void SubmitSprite(const std::string& spriteFilename, u32 screenX, u32 screenY, TextAlignment anchor = TextAlignment::ALIGN_TOP_LEFT);
        static void SubmitSprite(const std::string& spriteFilename, SpriteRect* dst = nullptr, SpriteRect* src = nullptr, TextAlignment anchor = TextAlignment::ALIGN_TOP_LEFT);

        static void OnWindowResize(uint32_t width, uint32_t height);

        static const std::unique_ptr<ShaderLibrary>& GetShaderLibrary();
    };

}