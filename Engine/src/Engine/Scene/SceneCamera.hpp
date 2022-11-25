#pragma once

#include "Engine/Renderer/Camera.hpp"

namespace rh {

    class SceneCamera : public Camera {
    public:
        enum class Type {
            Orthographic = 0,
            Perspective = 1
        };
    public:
        SceneCamera();
        virtual ~SceneCamera() = default;

        void SetViewportSize(u32 width, u32 height);
        inline void SetType(Type type) { m_Type = type; }

        void SetOrthographic(float size, float nearClip, float farClip);
        void SetOrthoGraphicSize(float size);
        inline float GetOrthoGraphicSize() const { return m_OrthographicSize; }

        void SetPerspective(float vFov, float nearClip, float farClip);
        void SetPerspectiveFoV(float vFov);
        inline float GetPerspectiveFoV() const { return m_PerspectiveVerticalFoV; }

    private:
        void RecalculateProjection();

    private:
        // Lens switch
        Type m_Type;

        // Orthographic values
        float m_OrthographicSize = 10.0f;
        float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

        // Perspective values
        float m_PerspectiveVerticalFoV = 45.0f;
        float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 100.0f;

        // Screen-based value
        float m_AspectRatio = 1.0f;
    };
}