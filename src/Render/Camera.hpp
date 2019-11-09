#ifndef CAMERA_H
#define CAMERA_H

#include "GameMath.hpp"

using namespace math;

class Camera {
public:
    Camera();

    vec3 position;
    scalar yaw, pitch, roll;

    void updateViewMatrix();
    void updateProjectionMatrix(f32 width, f32 height);

    Camera& lookAt(vec3 target, bool UpdateMatrix = false);

//private:
    mat4 viewMatrix;
    mat4 projectionMatrix;

    f32 m_fov;
    f32 m_zNear;
    f32 m_zFar;
    f32 m_aspectRatio;
};

#endif
