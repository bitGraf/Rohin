#pragma once
#include "Engine/Renderer/Framebuffer.hpp"

namespace Engine {

    class OpenGLFramebuffer : public Framebuffer {
    public:
        OpenGLFramebuffer(const FramebufferSpecification& spec);
        virtual ~OpenGLFramebuffer();

        void Invalidate();
        virtual void Resize(u32 width, u32 height) override;

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual u32 GetColorAttachmentID() const override { return m_ColorAttachment; }
        virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

    private:
        u32 m_FramebufferID = 0;
        u32 m_ColorAttachment = 0;
        u32 m_DepthAttachment = 0;
        FramebufferSpecification m_Specification;
    };

}