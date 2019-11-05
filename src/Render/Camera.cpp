#include "Camera.hpp"

Camera::Camera() : 
    m_fov(75),
    m_zNear(.1),
    m_zFar(100)
{
}

void Camera::updateViewMatrix() {
    yaw = 45;
    pitch = -15;
    roll = 0;

    position = vec3(2, 2, 2);

    viewMatrix = mat4(createYawPitchRollMatrix(yaw, roll, pitch).getTranspose()) *
        mat4(vec4(1,0,0,0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(-position,1));
}

void Camera::updateProjectionMatrix() {
    f32 AR = 800.0 / 600.0; // TODO: Should be input

    f32 tanHalf = tan(m_fov * d2r / 2);

    projectionMatrix = mat4(
        vec4(1 / (AR*tanHalf), 0, 0, 0),
        vec4(0, 1 / tanHalf, 0, 0),
        vec4(0, 0, -(m_zFar+m_zNear) / (m_zFar - m_zNear), -1),
        vec4(0, 0, -(2* m_zFar*m_zNear) / (m_zFar - m_zNear), 0)
    );
}