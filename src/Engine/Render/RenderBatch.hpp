#ifndef RENDER_BATCH_H
#define RENDER_BATCH_H

#define MAX_CALLS 1000

#include "glad\glad.h"
#include "GameMath.hpp"
#include "Scene\EnvironmentMap.hpp"
#include "Scene\Material.hpp"
class Scene;

using namespace math;

struct DrawCall {
    math::mat4 modelMatrix;
    GLuint numVerts = 0;
    GLuint VAO = 0;
    Material* mat;
};

struct RenderBatch {
    /* Common to all rendering */
    mat4 cameraViewProjectionMatrix;
    vec3 camPos;

    mat4 cameraView; // scene camera
    mat4 cameraProjection; //scene camera
    mat4 cameraModelMatrix; //scene camera
    vec3 viewPos; //Where you view from (Scene or debug camera)

    mat4 sunViewProjectionMatrix;

    DirLight *sun;
    PointLight *pointLights[4] = { nullptr };
    SpotLight *spotLights[4] = { nullptr };
    EnvironmentMap (*env);

    DrawCall calls[MAX_CALLS];
    u16 numCalls = 0;
};

#endif
