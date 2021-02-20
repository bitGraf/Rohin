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
        static Ref<Texture2D> Create(const std::string& path);
    };

    class TextureCube : public Texture {
    public:
        static Ref<TextureCube> Create(const std::string& path);
    };
}