#ifndef PICKABLE_H
#define PICKABLE_H

#include "GameMath.hpp"

using namespace math;

class Pickable {
public:
    Pickable();

    void calcScreenSpace(mat4 projectionMatrix, mat4 viewMatrix, mat4 modelMatrix, vec3 position, u32 width, u32 height);

//protected:
    vec3 screenPos;
    scalar pickRadius;
};

#endif //ifndef PLATFORM_H
