#include <enpch.hpp>
#include "Texture.hpp"

#include "Renderer.hpp"
#include "Engine/Platform/OpenGL/OpenGLTexture.hpp"

namespace Engine {

    Ref<Texture2D> Texture2D::Create(const std::string& path) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            ENGINE_LOG_ASSERT(false, "No API selected when creating vertexArray");
            return nullptr;
            break;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLTexture2D>(path);
            break;
        }

        ENGINE_LOG_ASSERT(false, "Unknown rendererAPI selected");
        return nullptr;
    }

    Ref<TextureCube> TextureCube::Create(const std::string& path) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            ENGINE_LOG_ASSERT(false, "No API selected when creating vertexArray");
            return nullptr;
            break;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLTextureCube>(path);
            break;
        }

        ENGINE_LOG_ASSERT(false, "Unknown rendererAPI selected");
        return nullptr;
    }
}