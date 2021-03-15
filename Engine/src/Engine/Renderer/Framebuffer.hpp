#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Core/GameMath.hpp"

namespace Engine {

    enum class FramebufferTextureFormat
    {
        None = 0,

        // Color
        RED8,
        //RGB8,
        RGBA8,
        RGBA16F,
        //RGB32F,
        //RG32F,

        // Depth/stencil
        //DEPTH32F,
        DEPTH24STENCIL8,

        // Defaults
        Depth = DEPTH24STENCIL8
    };

    struct FramebufferTextureSpecification
    {
        FramebufferTextureSpecification() = default;
        FramebufferTextureSpecification(FramebufferTextureFormat format) : TextureFormat(format) {}

        FramebufferTextureFormat TextureFormat;
        // TODO: add options for filtering/wrap
    };

    struct FramebufferAttachmentSpecification
    {
        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(const std::initializer_list<FramebufferTextureSpecification>& attachments)
            : Attachments(attachments) {}

        std::vector<FramebufferTextureSpecification> Attachments;
    };

    struct FramebufferSpecification {
        u32 Width = 1280;
        u32 Height = 720;
        math::vec4 ClearColor;
        FramebufferAttachmentSpecification Attachments;

        bool NoResize = false;
        bool SwapChainTarget = false; // true if screen buffer (no framebuffer)
    };

    class Framebuffer
    {
    public:
        virtual ~Framebuffer() = default;
        
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void Resize(u32 width, u32 height) = 0;
        virtual void BindTexture(u32 attachmentIndex = 0, u32 slot = 0) const = 0;

        virtual u32 GetWidth() const = 0;
        virtual u32 GetHeight() const = 0;

        // Get api-specific render IDs
        virtual u32 GetID() const = 0;
        virtual u32 GetColorAttachmentID(int index = 0) const = 0;
        virtual u32 GetDepthAttachmentID(int index = 0) const = 0;

        virtual const FramebufferSpecification& GetSpecification() const = 0;

        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
    };
}
