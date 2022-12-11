#include <enpch.hpp>
#include "Texture.hpp"

#include "Renderer.hpp"
#include "Engine/Platform/OpenGL/OpenGLTexture.hpp"

namespace rh {

    Texture2D* Texture2D::CreateAtLocation(void* ptr, const std::string& path) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            ENGINE_LOG_ASSERT(false, "No API selected when creating Texture2D");
            return nullptr;
            break;
        case RendererAPI::API::OpenGL:
            return new(ptr) OpenGLTexture2D(path);
            break;
        }

        ENGINE_LOG_ASSERT(false, "Unknown rendererAPI selected");
        return nullptr;
    }

    Texture2D* Texture2D::CreateAtLocation(void* ptr, const unsigned char* bitmap, u32 res) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            ENGINE_LOG_ASSERT(false, "No API selected when creating Texture2D");
            return nullptr;
            break;
        case RendererAPI::API::OpenGL:
            return new(ptr) OpenGLTexture2D(bitmap, res);
            break;
        }

        ENGINE_LOG_ASSERT(false, "Unknown rendererAPI selected");
        return nullptr;
    }

    TextureCube* TextureCube::CreateAtLocation(void* ptr, const std::string& path) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            ENGINE_LOG_ASSERT(false, "No API selected when creating Texture2D");
            return nullptr;
            break;
        case RendererAPI::API::OpenGL:
            return new(ptr) OpenGLTextureCube(path);
            break;
        }

        ENGINE_LOG_ASSERT(false, "Unknown rendererAPI selected");
        return nullptr;
    }


    Texture2D* Texture2D::Create(const std::string& path) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            ENGINE_LOG_ASSERT(false, "No API selected when creating Texture2D");
            return nullptr;
            break;
        case RendererAPI::API::OpenGL:
            return new OpenGLTexture2D(path);
            break;
        }
    
        ENGINE_LOG_ASSERT(false, "Unknown rendererAPI selected");
        return nullptr;
    }
    
    Texture2D* Texture2D::Create(const unsigned char* bitmap, u32 res) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            ENGINE_LOG_ASSERT(false, "No API selected when creating Texture2D");
            return nullptr;
            break;
        case RendererAPI::API::OpenGL:
            return new OpenGLTexture2D(bitmap, res);
            break;
        }
    
        ENGINE_LOG_ASSERT(false, "Unknown rendererAPI selected");
        return nullptr;
    }
    
    TextureCube* TextureCube::Create(const std::string& path) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            ENGINE_LOG_ASSERT(false, "No API selected when creating TextureCube");
            return nullptr;
            break;
        case RendererAPI::API::OpenGL:
            return new OpenGLTextureCube(path);
            break;
        }
    
        ENGINE_LOG_ASSERT(false, "Unknown rendererAPI selected");
        return nullptr;
    }
}