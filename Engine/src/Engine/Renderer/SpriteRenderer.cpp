#include <enpch.hpp>

#include "SpriteRenderer.hpp"

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Resources/MaterialCatalog.hpp"

namespace Engine {

    struct SpriteRendererData {
        std::unique_ptr<ShaderLibrary> ShaderLibrary;

        Ref<VertexArray> Quad;
        math::mat4 orthoMat;
    };

    static SpriteRendererData s_SpriteData;

    void SpriteRenderer::Init() {
        BENCHMARK_FUNCTION();

        s_SpriteData.ShaderLibrary = std::make_unique<ShaderLibrary>();
        auto textShader = GetShaderLibrary()->Load("Data/Shaders/Sprite.glsl");

        // create font-rendering globals
        s_SpriteData.orthoMat.orthoProjection(0, 1280, 720, 0, 1, -1);

        // initialize texture shader values
        textShader->Bind();
        textShader->SetMat4("r_orthoProjection", s_SpriteData.orthoMat);
        textShader->SetFloat("r_spriteTex", 0);

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

            u32 indices[6] = { 0, 2, 1, 0, 3, 2 };

            auto vbo = VertexBuffer::Create(data, 4 * sizeof(_vertex));
            vbo->SetLayout({
                { ShaderDataType::Float2, "a_Position" }
                });
            auto ebo = IndexBuffer::Create(indices, 6);

            s_SpriteData.Quad = VertexArray::Create();
            s_SpriteData.Quad->Bind();
            s_SpriteData.Quad->AddVertexBuffer(vbo);
            s_SpriteData.Quad->SetIndexBuffer(ebo);
            s_SpriteData.Quad->Unbind();
        }
    }

    void SpriteRenderer::Shutdown() {
    }

    void SpriteRenderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        s_SpriteData.orthoMat.orthoProjection(0, width, height, 0, -1, 1);
    }

    const std::unique_ptr<ShaderLibrary>& SpriteRenderer::GetShaderLibrary() {
        return s_SpriteData.ShaderLibrary;
    }

    std::pair<float,float> GetTextureOffset(f32 width, f32 height, TextAlignment anchor) {
        float xOff = 0.0f, yOff = 0.0f;
        switch (anchor) {
            case ALIGN_TOP_LEFT: {
                xOff = 0;
                yOff = 0;
            } break;
            case ALIGN_MID_LEFT: {
                xOff = 0;
                yOff = -height/2.0f;
            } break;
            case ALIGN_BOT_LEFT: {
                xOff = 0;
                yOff = -height;
            } break;
            case ALIGN_TOP_MID: {
                xOff = -width/2.0f;
                yOff = 0;
            } break;
            case ALIGN_MID_MID: {
                xOff = -width / 2.0f;
                yOff = -height/2.0f;
            } break;
            case ALIGN_BOT_MID: {
                xOff = -width / 2.0f;
                yOff = -height;
            } break;
            case ALIGN_TOP_RIGHT: {
                xOff = -width;
                yOff = 0;
            } break;
            case ALIGN_MID_RIGHT: {
                xOff = -width;
                yOff = -height/2.0f;
            } break;
            case ALIGN_BOT_RIGHT: {
                xOff = -width;
                yOff = -height;
            } break;
        }
        
        return std::make_pair(xOff, yOff);
    }

    void SpriteRenderer::SubmitSprite(const std::string& spriteFilename, u32 screenX, u32 screenY, TextAlignment anchor) {
        //SpriteRect src{ 1,1,0,0 };
        SpriteRect dst{ screenX, screenY, -1,-1 };
        SubmitSprite(spriteFilename, &dst, nullptr, anchor);
    }

    void SpriteRenderer::SubmitSprite(const std::string& spriteFilename, SpriteRect* dst, SpriteRect* src, TextAlignment anchor) {
        auto shader = s_SpriteData.ShaderLibrary->Get("Sprite");
        shader->Bind();

        auto tex = MaterialCatalog::GetTexture(spriteFilename);
        auto texWidth = (f32)tex->GetWidth();
        auto texHeight = (f32)tex->GetHeight();
        auto[xoff, yoff] = GetTextureOffset(texWidth, texHeight, anchor);
        
        // defaults
        math::vec4 _dst{ texWidth, texHeight, xoff, yoff };
        math::vec4 _src{ 1, 1, 0, 0 };

        // if rects are passed in
        if (dst) {
            if (dst->x1 > 0.0f)
                _dst.x = dst->x1 - dst->x0;
            if (dst->y1 > 0.0f)
                _dst.y = dst->y1 - dst->y0;

            auto[xoff_new, yoff_new] = GetTextureOffset(_dst.x, _dst.y, anchor);
            _dst.z = dst->x0 + xoff_new;
            _dst.w = dst->y0 + yoff_new;
        }
        if (src) {
            _src.x = src->x1 - src->x0;
            _src.y = src->y1 - src->y0;
            _src.z = src->x0;
            _src.w = src->y0;
        }

        shader->SetVec4("r_transform", _dst);
        shader->SetVec4("r_transformUV", _src);
        shader->SetMat4("r_orthoProjection", s_SpriteData.orthoMat);
        shader->SetVec3("r_textColor", math::vec3(1, 1, 1));

        tex->Bind(0);
        s_SpriteData.Quad->Bind();

        //RenderCommand::DisableDepthTest();
        RenderCommand::DrawIndexed(s_SpriteData.Quad, false);
        //RenderCommand::EnableDepthTest();
    }
}