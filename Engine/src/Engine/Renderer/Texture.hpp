#pragma once

namespace Engine {

    class Texture {
    public:
        virtual ~Texture() = default;

        virtual void Bind(u32 slot = 0) const = 0;
        virtual u32 GetID() const = 0;

        virtual u32 GetWidth() const = 0;
        virtual u32 GetHeight() const = 0;
    };

    class Texture2D : public Texture {
    public:
        static Texture2D* CreateAtLocation(void* ptr, const std::string& path);
        static Texture2D* CreateAtLocation(void* ptr, const unsigned char* bitmap, u32 res);

        static Texture2D* Create(const std::string& path);
        static Texture2D* Create(const unsigned char* bitmap, u32 res); // for text rendering
    };

    class TextureCube : public Texture {
    public:
        static TextureCube* CreateAtLocation(void* ptr, const std::string& path);

        static TextureCube* Create(const std::string& path);
    };
}