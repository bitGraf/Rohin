#pragma once

#include "Engine/Renderer/Buffer.hpp"

namespace Engine {

    /* Vertex Buffer *************************************************************/
    class OpenGLVertexBuffer : public VertexBuffer {
    public:
        OpenGLVertexBuffer(void* vertices, u32 size);
        virtual ~OpenGLVertexBuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
        virtual const BufferLayout& GetLayout() const override { return m_Layout; }

    private:
        u32 m_BufferID;
        BufferLayout m_Layout;
    };

    /* Index Buffer **************************************************************/
    class OpenGLIndexBuffer : public IndexBuffer {
    public:
        OpenGLIndexBuffer(void* indices, u32 count);
        virtual ~OpenGLIndexBuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;
        virtual u32 GetCount() const override { return m_Count; }

    private:
        u32 m_BufferID;
        u32 m_Count;
    };

}