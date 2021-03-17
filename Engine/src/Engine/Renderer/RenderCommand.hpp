#pragma once

#include "RendererAPI.hpp"

namespace Engine {

    class RenderCommand {
    public:
        inline static void SetClearColor(const math::vec4& color) {
            s_RendererAPI->SetClearColor(color);
        }
        inline static void Clear() {
            s_RendererAPI->Clear();
        }
        inline static void DrawLines(const Ref<VertexArray>& vertexArray, bool depth_test = true) {
            s_RendererAPI->DrawLines(vertexArray, depth_test);
        }
        inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, bool depth_test = true) {
            s_RendererAPI->DrawIndexed(vertexArray, depth_test);
        }
        inline static void DrawSubIndexed_points(u32 startIndex, u32 startVertex, u32 count) {
            s_RendererAPI->DrawSubIndexed_points(startIndex, startVertex, count);
        }
        inline static void DrawSubIndexed(u32 startIndex, u32 startVertex, u32 count) {
            s_RendererAPI->DrawSubIndexed(startIndex, startVertex, count);
        }
        inline static void Init() {
            s_RendererAPI->Init();
        }
        inline static void Shutdown() {
            s_RendererAPI->Shutdown();
        }
        inline static void SetViewport(u32 x, u32 y, u32 width, u32 height) {
            s_RendererAPI->SetViewport(x, y, width, height);
        }
        inline static void SetCullFront() {
            s_RendererAPI->SetCullFace(1);
        }
        inline static void SetCullBack() {
            s_RendererAPI->SetCullFace(-1);
        }
        inline static void SetCullNone() {
            s_RendererAPI->SetCullFace(0);
        }
        inline static void DisableDepthTest() {
            s_RendererAPI->SetDepthTest(false);
        }
        inline static void EnableDepthTest() {
            s_RendererAPI->SetDepthTest(true);
        }
        inline static void SetWireframe(bool enabled) {
            s_RendererAPI->SetWireframe(enabled);
        }
    private:
        static RendererAPI * s_RendererAPI;
    };
}