#pragma once

#include "RenderCommand.hpp"
#include "Shader.hpp"

namespace rh {

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

        static void SubmitLine(u32 screenX0, u32 screenY0, u32 screenX1, u32 screenY1, laml::Vec4 color);

        static void OnWindowResize(uint32_t width, uint32_t height);

        static const std::unique_ptr<ShaderLibrary>& GetShaderLibrary();
    };

}