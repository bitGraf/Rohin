#include <enpch.hpp>
#include "TextRenderer.hpp"

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Resources/DynamicFont.hpp"

#include <stb_truetype.h>

namespace Engine {

    struct TextRendererData {
        std::unique_ptr<ShaderLibrary> ShaderLibrary;

        Ref<VertexArray> TextQuad;
        math::mat4 orthoMat;

        std::unordered_map<std::string, DynamicFont*> fonts;
    };

    static TextRendererData s_Data;

    void TextRenderer::Init() {
        BENCHMARK_FUNCTION();

        s_Data.ShaderLibrary = std::make_unique<ShaderLibrary>();
        auto textShader = GetShaderLibrary()->Load("Data/Shaders/Text.glsl");

        // create font-rendering globals
        s_Data.orthoMat.orthoProjection(0, 1280, 720, 0, -1, 1);

        // initialize texture shader values
        textShader->Bind();
        textShader->SetMat4("r_orthoProjection", s_Data.orthoMat);
        textShader->SetFloat("r_fontTex", 0);

        // create Text quad
        {
            struct _vertex
            {
                math::vec2 Position;
            };

            _vertex* data = new _vertex[4];

            data[0].Position = math::vec2(0, 1);
            data[1].Position = math::vec2(0, 0);
            data[2].Position = math::vec2(1, 0);
            data[3].Position = math::vec2(1, 1);

            u32 indices[6] = { 0, 1, 2, 0, 2, 3 };

            auto vbo = VertexBuffer::Create(data, 4 * sizeof(_vertex));
            vbo->SetLayout({
                { ShaderDataType::Float2, "a_Position" }
                });
            auto ebo = IndexBuffer::Create(indices, 6);

            s_Data.TextQuad = VertexArray::Create();
            s_Data.TextQuad->Bind();
            s_Data.TextQuad->AddVertexBuffer(vbo);
            s_Data.TextQuad->SetIndexBuffer(ebo);
            s_Data.TextQuad->Unbind();
        }

        // load fonts
        s_Data.fonts.emplace("font_big", new DynamicFont());
        s_Data.fonts.emplace("font_medium", new DynamicFont());
        s_Data.fonts.emplace("font_small", new DynamicFont());

        s_Data.fonts["font_big"]->create("Data/Fonts/UbuntuMono-Regular.ttf", 48);
        s_Data.fonts["font_medium"]->create("Data/Fonts/UbuntuMono-Regular.ttf", 32);
        s_Data.fonts["font_small"]->create("Data/Fonts/UbuntuMono-Regular.ttf", 20);
    }

    void TextRenderer::Shutdown() {
    }

    //void TextRenderer::BeginTextRendering() {
    //    auto textShader = s_Data.ShaderLibrary->Get("Text");
    //    textShader->Bind();
    //    textShader->SetMat4("r_orthoProjection", s_Data.orthoMat);
    //}
    //
    //void TextRenderer::EndTextRendering() {
    //    Flush();
    //}
    //
    //void TextRenderer::Flush() {
    //    // currently doing nothing ^_^
    //}

    void TextRenderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        s_Data.orthoMat.orthoProjection(0, width, height, 0, -1, 1);
        //RenderCommand::SetViewport(0, 0, width, height);
    }

    const std::unique_ptr<ShaderLibrary>& TextRenderer::GetShaderLibrary() {
        return s_Data.ShaderLibrary;
    }

    void TextRenderer::SubmitText(const std::string& text, float startX, float startY, math::vec3 color, TextAlignment align) {
        SubmitText("font_small", text, startX, startY, color, align);
    }

    void TextRenderer::SubmitText(const std::string& fontName, const std::string& text, float startX, float startY, math::vec3 color, TextAlignment align) {
        BENCHMARK_FUNCTION();

        auto shader = s_Data.ShaderLibrary->Get("Text");
        shader->Bind();
        s_Data.TextQuad->Bind();
        if (s_Data.fonts.find(fontName) == s_Data.fonts.end()) {
            ENGINE_LOG_WARN("Could not draw text using font [{0}]", fontName);
            return;
        }
        auto font = s_Data.fonts.at(fontName);
        font->m_ftex->Bind();

        RenderCommand::SetCullFront();
        RenderCommand::DisableDepthTest();

        const char* _text = text.c_str();

        if (font->initialized) {
            float x = startX;
            float y = startY;

            float hOff, vOff;
            font->getTextOffset(&hOff, &vOff, align, font->getLength(_text), font->m_fontSize);

            shader->SetVec3("r_textColor", color);
            shader->SetMat4("r_orthoProjection", s_Data.orthoMat); // TODO: don't need these always

            while (*_text) {
                if (*_text == '\n') {
                    //Increase y by one line,
                    //reset x to start
                    x = startX;
                    y += font->m_fontSize;
                }
                if (*_text >= 32 && *_text < 128) {
                    stbtt_aligned_quad q;
                    char c = *_text - 32;
                    stbtt_GetBakedQuad(reinterpret_cast<stbtt_bakedchar*>(font->cdata), font->m_bitmapRes, font->m_bitmapRes, *_text - 32, &x, &y, &q, 1);//1=opengl & d3d10+,0=d3d9

                    float scaleX = q.x1 - q.x0;
                    float scaleY = q.y1 - q.y0;
                    float transX = q.x0;
                    float transY = q.y0;
                    shader->SetVec4("r_transform", math::vec4(scaleX, scaleY, transX + hOff, transY + vOff));

                    scaleX = q.s1 - q.s0;
                    scaleY = q.t1 - q.t0;
                    transX = q.s0;
                    transY = q.t0;
                    shader->SetVec4("r_transformUV", math::vec4(scaleX, scaleY, transX, transY));

                    RenderCommand::DrawIndexed(s_Data.TextQuad, false);
                }
                ++_text;
            }
        }

        RenderCommand::EnableDepthTest();
        RenderCommand::SetCullBack();
    }
}