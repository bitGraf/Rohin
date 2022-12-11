#include <enpch.hpp>
#include "OpenGLRendererAPI.hpp"

#include <glad/glad.h>

namespace rh {

    void OpenGLRendererAPI::SetClearColor(const laml::Vec4& color) {
        glClearColor(color.x, color.y, color.z, color.w);
    }

    void OpenGLRendererAPI::Clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRendererAPI::Init() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glFrontFace(GL_CCW);

        //glEnable(GL_MULTISAMPLE);
        //glEnable(GL_STENCIL_TEST);

        GLfloat pointSize;
        glGetFloatv(GL_POINT_SIZE, &pointSize);
        ENGINE_LOG_WARN("Default point size: {0}", pointSize);
        glPointSize(4);
        glGetFloatv(GL_POINT_SIZE, &pointSize);
        ENGINE_LOG_WARN("New point size: {0}", pointSize);

        GLfloat lineWidth;
        glGetFloatv(GL_LINE_WIDTH, &lineWidth);
        ENGINE_LOG_WARN("Default line width: {0}", lineWidth);
        glLineWidth(2);
        glGetFloatv(GL_LINE_WIDTH, &lineWidth);
        ENGINE_LOG_WARN("New line width: {0}", lineWidth);

        GLfloat maxAniso;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
        ENGINE_LOG_WARN("Max Anisotropy value: {0}", maxAniso);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void OpenGLRendererAPI::Shutdown() {
    }

    void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, bool depth_test) {
        if (!depth_test)
            glDisable(GL_DEPTH_TEST);

        glDrawElements(GL_LINES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

        if (!depth_test)
            glEnable(GL_DEPTH_TEST);
    }

    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, bool depth_test) {
        if (!depth_test)
            glDisable(GL_DEPTH_TEST);

        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

        if (!depth_test)
            glEnable(GL_DEPTH_TEST);
    }
    void OpenGLRendererAPI::DrawSubIndexed_points(u32 startIndex, u32 startVertex, u32 count) {
        glDrawElements(GL_POINTS, count, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * startIndex));
    }
    void OpenGLRendererAPI::DrawSubIndexed(u32 startIndex, u32 startVertex, u32 count) {
        //glDrawElementsBaseVertex(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * startIndex), startVertex);
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * startIndex));
    }

    void OpenGLRendererAPI::SetViewport(u32 x, u32 y, u32 width, u32 height) {
        glViewport(x, y, width, height);
    }

    void OpenGLRendererAPI::SetCullFace(int face) {
        switch (face) {
        case -1:
            glCullFace(GL_BACK);
            return;
        case 0:
            glCullFace(GL_FRONT_AND_BACK); // TODO: opposite of what I want actually
            return;
        case 1:
            glCullFace(GL_FRONT);
            return;
        default:
            ENGINE_LOG_ASSERT(false, "Not a valid face cull option");
        }
    }

    void OpenGLRendererAPI::SetDepthTest(bool enabled) {
        if (enabled) glEnable(GL_DEPTH_TEST);
        else         glDisable(GL_DEPTH_TEST);
    }

    void OpenGLRendererAPI::SetWireframe(bool enabled) {
        if (enabled) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}