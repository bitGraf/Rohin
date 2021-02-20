#pragma once

#include "Engine/Core/GameMath.hpp"

namespace Engine {

    class Camera {
    public:
        Camera() = default;
        Camera(const math::mat4& projection) : m_Projection(projection) {}
        virtual ~Camera() = default;

        const math::mat4& GetProjection() const { return m_Projection; }
    protected:
        math::mat4 m_Projection;
    };
}
