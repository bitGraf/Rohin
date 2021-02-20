#pragma once

#include "Engine/Renderer/Texture.hpp"

namespace Engine {

    class OpenGLTexture2D : public Texture2D {
    public:
        OpenGLTexture2D(const std::string& path);
        virtual ~OpenGLTexture2D();

        virtual void Bind(u32 slot = 0) const override;
        virtual u32 GetID() const override { return m_TextureID; }

        virtual u32 GetWidth() const override { return m_Width; }
        virtual u32 GetHeight() const override { return m_Height; }

    private:
        std::string m_Path;
        u32 m_Width;
        u32 m_Height;
        u32 m_TextureID;
    };

    class OpenGLTextureCube : public TextureCube {
    public:
        OpenGLTextureCube(const std::string& path);
        virtual ~OpenGLTextureCube();

        virtual void Bind(u32 slot = 0) const override;
        virtual u32 GetID() const override { return m_TextureID; }

        virtual u32 GetWidth() const override { return m_Width; }
        virtual u32 GetHeight() const override { return m_Height; }

    private:
        std::string m_Path;
        u32 m_Width;
        u32 m_Height;
        u32 m_TextureID;
    };
}
