#ifndef RENDER_BATCH_H
#define RENDER_BATCH_H

#define MAX_CALLS 1000

#include "glad/glad.h"
#include "Engine/Core/GameMath.hpp"
#include "Engine/Scene/EnvironmentMap.hpp"
#include "Engine/Scene/Material.hpp"
class Scene;

using namespace math;

struct DrawCall {
    math::mat4 modelMatrix;
    GLuint numVerts = 0;
    GLuint VAO = 0;
    Material* mat;
    UID_t id = 0;
};

struct RenderBatch {
    mat4 cameraView;
    mat4 cameraProjection;
    mat4 cameraModelMatrix;
    vec3 camPos;

    mat4 debugView; // where to draw the camera during debug mode

    mat4 sunViewProjectionMatrix;

    DirLight *sun;
    PointLight *pointLights[4] = { nullptr };
    SpotLight *spotLights[10] = { nullptr };
    EnvironmentMap (*env);

    DrawCall calls[MAX_CALLS];
    u16 numCalls = 0;
};

#endif
