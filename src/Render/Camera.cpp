#include "Camera.hpp"

Camera::Camera() : 
    m_fov(75),
    m_zNear(.1),
    m_zFar(100),
    m_aspectRatio(1280.0/720.0)
{
    yaw = 0;
    pitch = 0;
    roll = 0;
}

void Camera::updateViewMatrix() {
    viewMatrix = mat4(createYawPitchRollMatrix(yaw, pitch, roll).getTranspose()) *
        mat4(vec4(1,0,0,0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(-position,1));
}

void Camera::updateProjectionMatrix(f32 width, f32 height) {
    f32 tanHalf = tan(m_fov * d2r / 2);
    m_aspectRatio = width / height;

    projectionMatrix = mat4(
        vec4(1 / (m_aspectRatio*tanHalf), 0, 0, 0),
        vec4(0, 1 / tanHalf, 0, 0),
        vec4(0, 0, -(m_zFar+m_zNear) / (m_zFar - m_zNear), -1),
        vec4(0, 0, -(2* m_zFar*m_zNear) / (m_zFar - m_zNear), 0)
    ) * mat4(vec4(0,0,-1,0), vec4(0,1,0,0), vec4(1,0,0,0), vec4(0,0,0,1));
}

Camera& Camera::lookAt(vec3 target, bool UpdateMatrix) {
    vec3 forward = (target - position).get_unit();  // +X
    vec3 right;
    if (abs(forward.dot(vec3(0, 1, 0))) > 0.99) {
        right = forward.cross(vec3(0, 0, 1));      // +Z
    } else {
        right = forward.cross(vec3(0, 1, 0));      // +Z
    }
    vec3 up = right.cross(forward);                 // +Y

    /*pitch = asin(-up.y) * r2d;
    roll = atan2(right.y, up.y) * r2d;
    yaw = atan2(forward.x, forward.z) * r2d;*/

    pitch = asin(forward.y) * r2d;
    yaw = -atan2(forward.z, forward.x) * r2d;
    roll = -atan2(right.y, up.y) * r2d;

    if (UpdateMatrix) updateViewMatrix();

    return *this;
}