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

        DynamicFont font;
    };

    static TextRendererData s_Data;

    void TextRenderer::Init() {
        BENCHMARK_FUNCTION();

        s_Data.ShaderLibrary = std::make_unique<ShaderLibrary>();
        auto textShader = GetShaderLibrary()->Load("run_tree/Data/Shaders/Text.glsl");

        // create font-rendering globals
        s_Data.orthoMat.orthoProjection(0, 1280, 720, 0, -1, 1);

        // initialize texture shader values
        textShader->Bind();
        textShader->SetMat4("u_Projection", s_Data.orthoMat);
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
        s_Data.font.create("run_tree/Data/Fonts/UbuntuMono-Regular.ttf", 20);
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

    void TextRenderer::SubmitText(const std::string& text, float startX, float startY, math::vec3 color) {
        BENCHMARK_FUNCTION();

        auto shader = s_Data.ShaderLibrary->Get("Text");
        shader->Bind();
        s_Data.TextQuad->Bind();
        s_Data.font.m_ftex->Bind();

        RenderCommand::SetCullFront();
        RenderCommand::DisableDepthTest();

        auto& font = s_Data.font;
        auto align = TextAlignment::ALIGN_TOP_LEFT; // should pass in
        const char* _text = text.c_str();

        if (font.initialized) {
            float x = startX;
            float y = startY;

            float hOff, vOff;
            font.getTextOffset(&hOff, &vOff, align, font.getLength(_text), font.m_fontSize);

            shader->SetVec3("r_textColor", color);
            shader->SetMat4("r_orthoProjection", s_Data.orthoMat);

            while (*_text) {
                if (*_text == '\n') {
                    //Increase y by one line,
                    //reset x to start
                    x = startX;
                    y += font.m_fontSize;
                }
                if (*_text >= 32 && *_text < 128) {
                    stbtt_aligned_quad q;
                    char c = *_text - 32;
                    stbtt_GetBakedQuad(reinterpret_cast<stbtt_bakedchar*>(font.cdata), font.m_bitmapRes, font.m_bitmapRes, *_text - 32, &x, &y, &q, 1);//1=opengl & d3d10+,0=d3d9

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