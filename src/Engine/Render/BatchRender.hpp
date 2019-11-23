#ifndef RENDER_UTIL_H
#define RENDER_UTIL_H

#define MAX_CALLS 100

#include "glad\glad.h"
#include "GameMath.hpp"
#include "Scene\Lights.hpp"
#include "Scene\EnvironmentMap.hpp"
#include "Scene\Material.hpp"

using namespace math;

struct DrawCall {
    math::mat4 modelMatrix;
    GLuint numVerts = 0;
    GLuint VAO = 0;
    Material* mat;
};

struct BatchDrawCall {
    /* Common to all rendering */
    mat4 cameraViewProjectionMatrix;
    mat4 cameraView;
    mat4 cameraProjection;
    vec3 camPos;

    mat4 sunViewProjectionMatrix;

    DirLight* sun;
    PointLight (*pointLights)[4];
    SpotLight (*spotLights)[4];
    EnvironmentMap (*env);

    DrawCall calls[MAX_CALLS];
    u16 numCalls = 0;
};

#endif
