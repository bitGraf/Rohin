#include "enpch.hpp"
#include "OpenGLRendererAPI.hpp"

#include <glad/glad.h>

namespace Engine {

    void OpenGLRendererAPI::SetClearColor(const math::vec4& color) {
        glClearColor(color.x, color.y, color.z, color.w);
    }

    void OpenGLRendererAPI::Clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRendererAPI::Init() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glFrontFace(GL_CCW);

        //glEnable(GL_MULTISAMPLE);
        //glEnable(GL_STENCIL_TEST);
    }

    void OpenGLRendererAPI::Shutdown() {
    }

    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, bool depth_test) {
        if (!depth_test)
            glDisable(GL_DEPTH_TEST);

        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

        if (!depth_test)
            glEnable(GL_DEPTH_TEST);
    }
    void OpenGLRendererAPI::DrawSubIndexed(u32 startIndex, u32 startVertex, u32 count) {
        //glDrawElementsBaseVertex(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * startIndex), startVertex);
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * startIndex));
    }

    void OpenGLRendererAPI::SetViewport(u32 x, u32 y, u32 width, u32 height) {
        glViewport(x, y, width, height);
    }
}