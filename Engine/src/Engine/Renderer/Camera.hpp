#pragma once

#include "Engine/Core/Base.hpp"

namespace rh {

    class Camera {
    public:
        Camera() = default;
        Camera(const laml::Mat4& projection) : m_Projection(projection) {}
        virtual ~Camera() = default;

        const laml::Mat4& GetProjection() const { return m_Projection; }
    protected:
        laml::Mat4 m_Projection;
    };
}
