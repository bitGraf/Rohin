#include <enpch.hpp>

#include "SpriteRenderer.hpp"

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Resources/MaterialCatalog.hpp"

namespace rh {

    struct SpriteRendererData {
        std::unique_ptr<ShaderLibrary> ShaderLibrary;

        Ref<VertexArray> Quad;
        Ref<VertexArray> Line;
        laml::Mat4 orthoMat;
    };

    static SpriteRendererData s_SpriteData;

    void SpriteRenderer::Init() {
        BENCHMARK_FUNCTION();

        s_SpriteData.ShaderLibrary = std::make_unique<ShaderLibrary>();
        auto textShader = GetShaderLibrary()->Load("Data/Shaders/Sprite.glsl");
        auto line2DShader = GetShaderLibrary()->Load("Data/Shaders/Line2D.glsl");

        // create font-rendering globals
        laml::transform::create_projection_orthographic(s_SpriteData.orthoMat, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f, -1.0f);

        // initialize texture shader values
        textShader->Bind();
        textShader->SetMat4("r_orthoProjection", s_SpriteData.orthoMat);
        textShader->SetFloat("r_spriteTex", 0);
        // initialize line shader values
        line2DShader->Bind();
        line2DShader->SetMat4("r_orthoProjection", s_SpriteData.orthoMat);

        // create Text quad
        {
            struct _vertex
            {
                laml::Vec2 Position;
            };

            _vertex* data = new _vertex[4];

            data[0].Position = laml::Vec2(0, 1);
            data[1].Position = laml::Vec2(0, 0);
            data[2].Position = laml::Vec2(1, 0);
            data[3].Position = laml::Vec2(1, 1);

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

        // create Line
        {
            struct _vertex
            {
                float Position; // can we remove this entirely?
            };

            _vertex* data = new _vertex[2];

            data[0].Position = 0;
            data[1].Position = 1;

            u32 indices[2] = { 0, 1 };

            auto vbo = VertexBuffer::Create(data, 2 * sizeof(_vertex));
            vbo->SetLayout({
                { ShaderDataType::Float, "a_Position" }
                });
            auto ebo = IndexBuffer::Create(indices, 2);

            s_SpriteData.Line = VertexArray::Create();
            s_SpriteData.Line->Bind();
            s_SpriteData.Line->AddVertexBuffer(vbo);
            s_SpriteData.Line->SetIndexBuffer(ebo);
            s_SpriteData.Line->Unbind();
        }
    }

    void SpriteRenderer::Shutdown() {
    }

    void SpriteRenderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        laml::transform::create_projection_orthographic(s_SpriteData.orthoMat, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
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
        laml::Vec4 _dst{ texWidth, texHeight, xoff, yoff };
        laml::Vec4 _src{ 1.0f, 1.0f, 0.0f, 0.0f };

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
        shader->SetVec3("r_textColor", laml::Vec3(1, 1, 1));

        tex->Bind(0);
        s_SpriteData.Quad->Bind();

        //RenderCommand::DisableDepthTest();
        RenderCommand::DrawIndexed(s_SpriteData.Quad, false);
        //RenderCommand::EnableDepthTest();
    }

    void SpriteRenderer::SubmitLine(u32 screenX0, u32 screenY0, u32 screenX1, u32 screenY1, laml::Vec4 color) {
        auto shader = s_SpriteData.ShaderLibrary->Get("Line2D");
        shader->Bind();

        // defaults
        laml::Vec2 verts[2] { { (f32)screenX0, (f32)screenY0}, {(f32)screenX1, (f32)screenY1 } };

        shader->SetVec2("r_verts[0]", verts[0]);
        shader->SetVec2("r_verts[1]", verts[1]);
        shader->SetMat4("r_orthoProjection", s_SpriteData.orthoMat);
        shader->SetVec4("r_Color", color);

        s_SpriteData.Line->Bind();
        RenderCommand::DrawLines(s_SpriteData.Line, false);
    }
}