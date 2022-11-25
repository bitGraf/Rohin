#include <enpch.hpp>
#include "Framebuffer.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Platform/OpenGL/OpenGLFramebuffer.hpp"

namespace rh {

    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            ENGINE_LOG_ASSERT(false, "No API selected when creating vertexBuffer");
            return nullptr;
            break;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLFramebuffer>(spec);
            break;
        }

        ENGINE_LOG_ASSERT(false, "Unknown rendererAPI selected");
        return nullptr;
    }
}
