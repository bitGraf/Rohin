#include <enpch.hpp>
#include "OpenGLFramebuffer.hpp"

#include <glad/glad.h>

namespace Engine {

    namespace Utils {
        static bool IsDepthFormat(FramebufferTextureFormat format) {
            switch (format) {
                case FramebufferTextureFormat::DEPTH24STENCIL8:
                case FramebufferTextureFormat::DEPTH32F:
                    return true;
            }
            return false;
        }

        static GLenum DataType(GLenum format)
        {
            switch (format)
            {
            case GL_RGBA8: return GL_UNSIGNED_BYTE;
            case GL_RG16F:
            case GL_RG32F:
            case GL_RGBA16F:
            case GL_RGBA32F: return GL_FLOAT;
            case GL_DEPTH24_STENCIL8: return GL_UNSIGNED_INT_24_8;
            }

            ENGINE_LOG_ASSERT(false, "Unknown format!");
            return 0;
        }

        static void AttachColorTexture(u32 id, GLenum format, uint32_t width, uint32_t height, int index)
        {
            // Only RGBA access for now
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, DataType(format), nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, id, 0);
        }

        static void AttachDepthTexture(u32 id, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
        {
            glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, id, 0);
        }
    }

    OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification & spec) 
        : m_Specification(spec), m_Width(spec.Width), m_Height(spec.Height) {

        for (auto format : m_Specification.Attachments.Attachments) {
            if (!Utils::IsDepthFormat(format.TextureFormat)) {
                m_ColorAttachmentFormats.emplace_back(format.TextureFormat);
            } else {
                m_DepthAttachmentFormat = format.TextureFormat;
            }
        }

        Invalidate();
    }

    OpenGLFramebuffer::~OpenGLFramebuffer() {
        glDeleteFramebuffers(1, &m_FramebufferID);
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);

        m_ColorAttachments.clear();
    }

    void OpenGLFramebuffer::Invalidate() {

        if (m_FramebufferID) {
            glDeleteFramebuffers(1, &m_FramebufferID);
            glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
            glDeleteTextures(1, &m_DepthAttachment);

            m_ColorAttachments.clear();
        }

        if (m_ColorAttachmentFormats.size()) {
            m_ColorAttachments.resize(m_ColorAttachmentFormats.size());
            glCreateTextures(GL_TEXTURE_2D, m_ColorAttachments.size(), m_ColorAttachments.data()); // he?

            for (int n = 0; n < m_ColorAttachments.size(); n++) {
                glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[n]);
                switch (m_ColorAttachmentFormats[n]) {
                case FramebufferTextureFormat::RGBA8:
                    Utils::AttachColorTexture(m_ColorAttachments[n], GL_RGBA8, m_Width, m_Height, n);
                    break;
                case FramebufferTextureFormat::RGBA16F:
                    Utils::AttachColorTexture(m_ColorAttachments[n], GL_RGBA16F, m_Width, m_Height, n);
                    break;
                case FramebufferTextureFormat::RGBA32F:
                    Utils::AttachColorTexture(m_ColorAttachments[n], GL_RGBA32F, m_Width, m_Height, n);
                    break;
                case FramebufferTextureFormat::RG32F:
                    Utils::AttachColorTexture(m_ColorAttachments[n], GL_RG32F, m_Width, m_Height, n);
                    break;
                }
            }
        }

        if (m_DepthAttachmentFormat != FramebufferTextureFormat::None) {
            glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
            glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
            switch (m_DepthAttachmentFormat) {
            case FramebufferTextureFormat::DEPTH24STENCIL8:
                Utils::AttachDepthTexture(m_DepthAttachment, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Width, m_Height);
                break;
            case FramebufferTextureFormat::DEPTH32F:
                Utils::AttachDepthTexture(m_DepthAttachment, GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT, m_Width, m_Height);
                break;
            }

        }

        if (m_ColorAttachments.size() > 1) {
            ENGINE_LOG_ASSERT(m_ColorAttachments.size() <= 4, "More color attachments not supported atm");
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers(m_ColorAttachments.size(), buffers);
        }
        else if (m_ColorAttachments.size() == 0) {
            // only a depth-pass
            glDrawBuffer(GL_NONE);
        }

        ENGINE_LOG_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFramebuffer::Resize(u32 width, u32 height) {
        m_Specification.Width = width;
        m_Specification.Height = height;
        Invalidate();
    }

    void OpenGLFramebuffer::Bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
        glViewport(0, 0, m_Specification.Width, m_Specification.Height);
    }

    void OpenGLFramebuffer::Unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFramebuffer::BindTexture(u32 attachmentIndex, u32 slot) const
    {
        glBindTextureUnit(slot, m_ColorAttachments[attachmentIndex]);
    }
}