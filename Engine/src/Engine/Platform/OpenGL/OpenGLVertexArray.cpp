#include <enpch.hpp>
#include "OpenGLVertexArray.hpp"

#include <glad/glad.h>

namespace rh {

    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
        switch (type) {
        case ShaderDataType::Float:  return GL_FLOAT;
        case ShaderDataType::Float2: return GL_FLOAT;
        case ShaderDataType::Float3: return GL_FLOAT;
        case ShaderDataType::Float4: return GL_FLOAT;
        case ShaderDataType::Int:    return GL_INT;
        case ShaderDataType::Int2:   return GL_INT;
        case ShaderDataType::Int3:   return GL_INT;
        case ShaderDataType::Int4:   return GL_INT;
        case ShaderDataType::Mat3:   return GL_FLOAT;
        case ShaderDataType::Mat4:   return GL_FLOAT;
        case ShaderDataType::Bool:   return GL_BOOL;
        }

        ENGINE_LOG_ASSERT(false, "Unknown ShaderDataType");
        return 0;
    }

    static bool IsIntegerType(ShaderDataType type) {
        switch (type) {
        case ShaderDataType::Int:    return true;
        case ShaderDataType::Int2:   return true;
        case ShaderDataType::Int3:   return true;
        case ShaderDataType::Int4:   return true;

        case ShaderDataType::Float:  return false;
        case ShaderDataType::Float2: return false;
        case ShaderDataType::Float3: return false;
        case ShaderDataType::Float4: return false;
        case ShaderDataType::Mat3:   return false;
        case ShaderDataType::Mat4:   return false;
        case ShaderDataType::Bool:   return false;
        }

        ENGINE_LOG_ASSERT(false, "Unknown ShaderDataType");
        return 0;
    }

    OpenGLVertexArray::OpenGLVertexArray() {
        glCreateVertexArrays(1, &m_ArrayID);
    }

    OpenGLVertexArray::~OpenGLVertexArray() {
        glDeleteVertexArrays(1, &m_ArrayID);
    }

    void OpenGLVertexArray::Bind() const {
        glBindVertexArray(m_ArrayID);
    }
    void OpenGLVertexArray::Unbind() const {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
        glBindVertexArray(m_ArrayID);
        vertexBuffer->Bind();

        ENGINE_LOG_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "VertexBuffer has no layout");

        u32 index = 0;
        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout) {
            glEnableVertexAttribArray(index);
            if (IsIntegerType(element.Type)) {
                glVertexAttribIPointer(index,
                    element.GetComponentCount(),
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    layout.GetStride(),
                    (const void*)element.Offset);
            }
            else {
                glVertexAttribPointer(index,
                    element.GetComponentCount(),
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)element.Offset);
            }
            index++;
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }
    void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
        glBindVertexArray(m_ArrayID);
        indexBuffer->Bind();

        m_IndexBuffer = indexBuffer;
    }

}