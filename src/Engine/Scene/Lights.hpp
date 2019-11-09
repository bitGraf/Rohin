#ifndef LIGHTS_H
#define LIGHTS_H

#include "GameMath.hpp"

using namespace math;

class Light {
public:
    vec4 color;
    scalar strength;

//private:
    Light() {}
};

class PointLight : public Light {
public:
    vec3 position;
};

class DirLight : public Light {
public:
    vec3 direction;
};

class SpotLight : public Light {
public:
    vec3 position;
    vec3 direction;
    f32 inner_cutoff;
    f32 outer_cutoff;
};

#endif