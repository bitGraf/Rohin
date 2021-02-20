#include "enpch.hpp"
#include "Buffer.hpp"

#include "Renderer.hpp"

/* API implementations */
#include "Engine/Platform/OpenGL/OpenGLBuffer.hpp"

namespace Engine {
    Ref<IndexBuffer> IndexBuffer::Create(void* indices, u32 count) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            ENGINE_LOG_ASSERT(false, "No API selected when creating vertexBuffer");
            return nullptr;
            break;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLIndexBuffer>(indices, count);
            break;
        }

        ENGINE_LOG_ASSERT(false, "Unknown rendererAPI selected");
        return nullptr;
    }

    Ref<VertexBuffer> VertexBuffer::Create(void* vertices, u32 size) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            ENGINE_LOG_ASSERT(false, "No API selected when creating indexBuffer");
            return nullptr;
            break;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLVertexBuffer>(vertices, size);
            break;
        }

        ENGINE_LOG_ASSERT(false, "Unknown rendererAPI selected");
        return nullptr;
    }

}