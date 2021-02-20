#include <enpch.hpp>
#include "VertexArray.hpp"
#include "Renderer.hpp"

#include "Engine/Platform/OpenGL/OpenGLVertexArray.hpp"

namespace Engine {

    Ref<VertexArray> VertexArray::Create() {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            ENGINE_LOG_ASSERT(false, "No API selected when creating vertexArray");
            return nullptr;
            break;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLVertexArray>();
            break;
        }

        ENGINE_LOG_ASSERT(false, "Unknown rendererAPI selected");
        return nullptr;
    }
}