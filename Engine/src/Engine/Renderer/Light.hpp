#pragma once

#include "Engine/Core/Base.hpp"

namespace rh {

    enum LightType {
        Directional = 0,
        Point,
        Spot
    };

    class Light {
    public:
        laml::Vec3 position;
        laml::Vec3 direction;
        laml::Vec3 color;
        laml::Scalar strength;
        laml::Scalar inner, outer;
        LightType type;
    };
}
