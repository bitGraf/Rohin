#include <enpch.hpp>
#include "SceneCamera.hpp"

namespace Engine {

    SceneCamera::SceneCamera() : m_Type(Type::Orthographic) {
        RecalculateProjection();
    }
    
    void SceneCamera::SetViewportSize(u32 width, u32 height) {
        m_AspectRatio = (float)width / (float)height;
        RecalculateProjection();
    }

    void SceneCamera::SetOrthographic(float size, float nearClip, float farClip) {
        m_OrthographicSize = size;
        m_OrthographicNear = nearClip;
        m_OrthographicFar = farClip;

        m_Type = Type::Orthographic;

        RecalculateProjection();
    }
    
    void SceneCamera::SetOrthoGraphicSize(float size) { 
        SetOrthographic(size, m_OrthographicNear, m_OrthographicFar);
    }

    void SceneCamera::SetPerspective(float vFov, float nearClip, float farClip) {
        m_PerspectiveVerticalFoV = vFov;
        m_PerspectiveNear = nearClip;
        m_PerspectiveFar = farClip;

        m_Type = Type::Perspective;

        RecalculateProjection();
    }

    void SceneCamera::SetPerspectiveFoV(float vFov) {
        SetPerspective(vFov, m_PerspectiveNear, m_PerspectiveFar);
    }

    void SceneCamera::RecalculateProjection() {
        switch (m_Type) {
            case Type::Orthographic: {
                float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
                float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
                float orthoTop = m_OrthographicSize * 0.5f;
                float orthoBot = -m_OrthographicSize * 0.5f;

                m_Projection.orthoProjection(orthoLeft, orthoRight, orthoBot, orthoTop, m_OrthographicNear, m_OrthographicFar);
                return;
            }

            case Type::Perspective: {
                f32 tanHalf = tan(m_PerspectiveVerticalFoV * math::d2r / 2);

                m_Projection = math::mat4(
                    math::vec4(1 / (m_AspectRatio*tanHalf), 0, 0, 0),
                    math::vec4(0, 1 / tanHalf, 0, 0),
                    math::vec4(0, 0, -(m_PerspectiveFar + m_PerspectiveNear) / (m_PerspectiveFar - m_PerspectiveNear), -1),
                    math::vec4(0, 0, -(2 * m_PerspectiveFar*m_PerspectiveNear) / (m_PerspectiveFar - m_PerspectiveNear), 0)
                );

                return;
            }
        }

        ENGINE_LOG_ERROR("Impossible camera type O_o");
    }

}
