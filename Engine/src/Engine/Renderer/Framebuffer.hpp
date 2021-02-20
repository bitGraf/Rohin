#pragma once

#include "Engine/Core/Base.hpp"

namespace Engine {

    struct FramebufferSpecification {
        u32 Width, Height;
        //FramebufferFormat Format = 
        u32 Samples = 1;

        bool SwapChainTarget = false;
    };

    class Framebuffer
    {
    public:
        virtual ~Framebuffer() = default;
        
        virtual const FramebufferSpecification& GetSpecification() const = 0;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void Resize(u32 width, u32 height) = 0;

        virtual u32 GetColorAttachmentID() const = 0;

        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
    };
}
