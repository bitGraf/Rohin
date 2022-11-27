#include <enpch.hpp>
#include "SceneCamera.hpp"

namespace rh {

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

                laml::transform::create_projection_orthographic(m_Projection, 
                    orthoLeft, orthoRight, orthoBot, orthoTop, m_OrthographicNear, m_OrthographicFar);
                return;
            }

            case Type::Perspective: {
                //f32 tanHalf = tan(m_PerspectiveVerticalFoV * laml::constants::deg2rad / 2.0);
                //m_Projection = laml::Mat4(
                //    laml::Vec4(1 / (m_AspectRatio*tanHalf), 0, 0, 0),
                //    laml::Vec4(0, 1 / tanHalf, 0, 0),
                //    laml::Vec4(0, 0, -(m_PerspectiveFar + m_PerspectiveNear) / (m_PerspectiveFar - m_PerspectiveNear), -1),
                //    laml::Vec4(0, 0, -(2 * m_PerspectiveFar*m_PerspectiveNear) / (m_PerspectiveFar - m_PerspectiveNear), 0)
                //);
                laml::transform::create_projection_perspective(m_Projection, 
                    m_PerspectiveVerticalFoV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);

                return;
            }
        }

        ENGINE_LOG_ERROR("Impossible camera type O_o");
    }

}
