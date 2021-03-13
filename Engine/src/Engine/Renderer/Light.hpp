#pragma once

#include "Engine/Core/GameMath.hpp"

namespace Engine {

    enum LightType {
        Directional = 0,
        Point,
        Spot
    };

    class Light {
    public:
        math::vec3 position;
        math::vec3 direction;
        math::vec3 color;
        float strength;
        float inner, outer;
        LightType type;
    };
}
