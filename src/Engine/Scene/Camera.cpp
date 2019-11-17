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

    playerControlled = false;
}

void Camera::init() {
    Input::watchKey("key_w", GLFW_KEY_W);
    Input::watchKey("key_a", GLFW_KEY_A);
    Input::watchKey("key_s", GLFW_KEY_S);
    Input::watchKey("key_d", GLFW_KEY_D);
    Input::watchKey("key_space", GLFW_KEY_SPACE);
    Input::watchKey("key_shift", GLFW_KEY_LEFT_SHIFT);

    Input::watchKey("key_up", GLFW_KEY_UP);
    Input::watchKey("key_down", GLFW_KEY_DOWN);
    Input::watchKey("key_left", GLFW_KEY_LEFT);
    Input::watchKey("key_right", GLFW_KEY_RIGHT);
    Input::watchKey("key_numpad0", GLFW_KEY_KP_0);
    Input::watchKey("key_rctrl", GLFW_KEY_RIGHT_CONTROL);
}

void Camera::update(double dt) {
    if (playerControlled) {
        vec3 vel;
        f32 yawChange = 0;
        f32 pitchChange = 0;
        f32 rollChange = 0;

        f32 speed = 10;
        f32 yawRate = 60;
        f32 pitchRate = 60;
        f32 rollRate = 60;

        vec3 forward = vec3(viewMatrix.row1()).get_unit();
        vec3 up = vec3(viewMatrix.row2()).get_unit();
        vec3 right = vec3(viewMatrix.row3()).get_unit();

        if (Input::getKeyState("key_w")) {
            vel += forward;
        }
        if (Input::getKeyState("key_s")) {
            vel -= forward;
        }
        if (Input::getKeyState("key_a")) {
            vel -= right;
        }
        if (Input::getKeyState("key_d")) {
            vel += right;
        }
        if (Input::getKeyState("key_space")) {
            vel += up;
        }
        if (Input::getKeyState("key_shift")) {
            vel -= up;
        }

        if (Input::getKeyState("key_up")) {
            pitchChange += 1;
        }
        if (Input::getKeyState("key_down")) {
            pitchChange -= 1;
        }
        if (Input::getKeyState("key_left")) {
            yawChange += 1;
        }
        if (Input::getKeyState("key_right")) {
            yawChange -= 1;
        }
        if (Input::getKeyState("key_numpad0")) {
            rollChange += 1;
        }
        if (Input::getKeyState("key_rctrl")) {
            rollChange -= 1;
        }

        vel *= speed;
        yawChange *= yawRate;
        pitchChange *= pitchRate;
        rollChange *= rollRate;

        position += vel * dt;
        yaw += yawChange * dt;
        pitch += pitchChange * dt;
        roll += rollChange * dt;
    }
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

    pitch = asin(forward.y) * r2d;
    yaw = -atan2(forward.z, forward.x) * r2d;
    roll = -atan2(right.y, up.y) * r2d;

    if (UpdateMatrix) updateViewMatrix();

    return *this;
}