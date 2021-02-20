#pragma once

#include "Engine/Core/Base.hpp"

namespace Engine {

    enum class ShaderDataType : u8 {
        None = 0, 
        Float, Float2, Float3, Float4,
        Mat3, Mat4, 
        Int, Int2, Int3, Int4,
        Bool
    };

    static u32 ShaderDataTypeSize(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float:  return 4;          break;
            case ShaderDataType::Float2: return 4 * 2;      break;
            case ShaderDataType::Float3: return 4 * 3;      break;
            case ShaderDataType::Float4: return 4 * 4;      break;
            case ShaderDataType::Int:    return 4;          break;
            case ShaderDataType::Int2:   return 4 * 2;      break;
            case ShaderDataType::Int3:   return 4 * 3;      break;
            case ShaderDataType::Int4:   return 4 * 4;      break;
            case ShaderDataType::Mat3:   return 4 * 3 * 3; break;
            case ShaderDataType::Mat4:   return 4 * 4 * 4; break;
            case ShaderDataType::Bool:   return 4;         break;
        }

        ENGINE_LOG_ASSERT(false, "Unknown ShaderDataType");
        return 0;
    }

    struct BufferElement {
        BufferElement() {}

        ShaderDataType Type;
        std::string Name;
        u32 Size;
        u32 Offset;
        bool Normalized;

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
            : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {
        }

        u32 GetComponentCount() const {
            switch (Type) {
                case ShaderDataType::Float:  return 1;      break;
                case ShaderDataType::Float2: return 2;      break;
                case ShaderDataType::Float3: return 3;      break;
                case ShaderDataType::Float4: return 4;      break;
                case ShaderDataType::Int:    return 1;      break;
                case ShaderDataType::Int2:   return 2;      break;
                case ShaderDataType::Int3:   return 3;      break;
                case ShaderDataType::Int4:   return 4;      break;
                case ShaderDataType::Mat3:   return 3 * 3;  break;
                case ShaderDataType::Mat4:   return 4 * 4;  break;
                case ShaderDataType::Bool:   return 1;      break;
            }

            ENGINE_LOG_ASSERT(false, "Unknown ShaderDataType");
            return 0;
        }
    };

    class BufferLayout {
    public:
        BufferLayout() {}
        BufferLayout(const std::initializer_list<BufferElement>& elements) : m_Elements(elements) {
            CalculateOffsetsAndStride();
        }

        inline u32 GetStride() const { return m_Stride; }
        inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
    private:
        void CalculateOffsetsAndStride() {
            u32 offset = 0;
            m_Stride = 0;
            for (auto& element : m_Elements) {
                element.Offset = offset;
                offset += element.Size;
                m_Stride += element.Size;
            }
        }
        std::vector<BufferElement> m_Elements;
        u32 m_Stride = 0;
    };

    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() {}

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetLayout(const BufferLayout& layout) = 0;
        virtual const BufferLayout& GetLayout() const = 0;

        static Ref<VertexBuffer> Create(void* vertices, u32 size);
    };

    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() {}

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual u32 GetCount() const = 0;

        static Ref<IndexBuffer> Create(void* indices, u32 count);
    };

}