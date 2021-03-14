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

        virtual void BindTexture(u32 attachmentIndex = 0, u32 slot = 0) const override;

        virtual u32 GetWidth() const override { return m_Specification.Width; }
        virtual u32 GetHeight() const override { return m_Specification.Height; }

        virtual u32 GetID() const override { return m_FramebufferID; }
        virtual u32 GetColorAttachmentID(int index = 0) const override { return m_ColorAttachments[index]; };
        virtual u32 GetDepthAttachmentID(int index = 0) const override { return m_DepthAttachment; }

        virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

    private:
        FramebufferSpecification m_Specification;
        u32 m_FramebufferID = 0;

        std::vector<u32> m_ColorAttachments;
        u32 m_DepthAttachment;

        std::vector<FramebufferTextureFormat> m_ColorAttachmentFormats;
        FramebufferTextureFormat m_DepthAttachmentFormat = FramebufferTextureFormat::None;

        u32 m_Width = 0, m_Height = 0;
    };

}