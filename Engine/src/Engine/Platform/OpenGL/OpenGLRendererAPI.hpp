#pragma once

#include "Engine/Renderer/RendererAPI.hpp"

namespace rh {

    class OpenGLRendererAPI : public RendererAPI {
    public:
        virtual void SetClearColor(const laml::Vec4& color) override;
        virtual void Clear() override;

        virtual void Init() override;
        virtual void Shutdown() override;

        virtual void DrawLines(const Ref<VertexArray>& vertexArray, bool depth_test) override;
        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, bool depth_test) override;
        virtual void DrawSubIndexed_points(u32 startIndex, u32 startVertex, u32 count) override;
        virtual void DrawSubIndexed(u32 startIndex, u32 startVertex, u32 count) override;
        virtual void SetViewport(u32 x, u32 y, u32 width, u32 height) override;

        virtual void SetCullFace(int face) override;
        virtual void SetDepthTest(bool enabled) override;
        virtual void SetWireframe(bool enabled) override;
    };
}
