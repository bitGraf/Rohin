#pragma once

#include "Engine/Core/GameMath.hpp"
#include "VertexArray.hpp"

namespace Engine {
    
    class RendererAPI {
    public:
        enum class API {
            None = 0, OpenGL = 1
        };

    public:
        virtual void SetClearColor(const math::vec4& color) = 0;
        virtual void Clear() = 0;

        virtual void Init() = 0;
        virtual void Shutdown() = 0;

        virtual void DrawLines(const Ref<VertexArray>& vertexArray, bool depth_test) = 0;
        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, bool depth_test) = 0;
        virtual void DrawSubIndexed(u32 startIndex, u32 startVertex, u32 count) = 0;
        virtual void DrawSubIndexed_points(u32 startIndex, u32 startVertex, u32 count) = 0;
        virtual void SetViewport(u32 x, u32 y, u32 width, u32 height) = 0;

        virtual void SetCullFace(int face) = 0;
        virtual void SetDepthTest(bool enabled) = 0;

        static inline API GetAPI() { return s_API; }

    private:
        static API s_API;
    };
}