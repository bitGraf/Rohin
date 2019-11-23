#include "Camera.hpp"

Camera::Camera() : 
    m_fov(75),
    m_zNear(.1),
    m_zFar(100),
    m_aspectRatio(800/600)
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

        f32 horizSens = 0.1;
        f32 vertSens = 0.1;

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

        yawChange = -Input::m_mouseMove.x * horizSens;
        pitchChange = -Input::m_mouseMove.y * vertSens;
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

    yaw = fmod(yaw, 360);
    pitch = fmod(pitch, 360);
}

void Camera::updateViewMatrix() {
    viewMatrix = mat4(createYawPitchRollMatrix(yaw, pitch, roll).getTranspose()) *
        mat4(vec4(1,0,0,0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(-position,1));
}

/*
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
*/

void Camera::updateViewFrustum(f32 width, f32 height) {
    updateViewMatrix();

    f32 tanHalf = tan(m_fov * d2r / 2);
    m_aspectRatio = width / height;

    projectionMatrix = mat4(
        vec4(1 / (m_aspectRatio*tanHalf), 0, 0, 0),
        vec4(0, 1 / tanHalf, 0, 0),
        vec4(0, 0, -(m_zFar + m_zNear) / (m_zFar - m_zNear), -1),
        vec4(0, 0, -(2 * m_zFar*m_zNear) / (m_zFar - m_zNear), 0)
    ) * mat4(vec4(0, 0, -1, 0), vec4(0, 1, 0, 0), vec4(1, 0, 0, 0), vec4(0, 0, 0, 1));

    scalar vertFOV = m_fov * d2r; //radians
    scalar horizFOV = 2 * atan(m_aspectRatio*tanHalf); //radians

    scalar s_h = sin(horizFOV / 2);
    scalar c_h = cos(horizFOV / 2);
    scalar s_v = sin(vertFOV / 2);
    scalar c_v = cos(vertFOV / 2);

    mat4 vv = mat4(viewMatrix.row1(), viewMatrix.row2(), viewMatrix.row3(), viewMatrix.row4());

    frustum.top = vv * vec4(-s_v, c_v, 0, 0);
    frustum.bottom = vv * vec4(-s_v, -c_v, 0, 0);

    frustum.left = vv * vec4(-s_h, 0, -c_h, 0);
    frustum.right = vv * vec4(-s_h, 0, c_h, 0);

    frustum.zNear = vv * vec4(-1, 0, 0,  m_zNear);
    frustum.zFar  = vv * vec4( 1, 0, 0, -m_zFar);

    /*mat4 vp = projectionMatrix * viewMatrix;

    frustum.zNear   = -(vp.col4() - vp.col1());
    frustum.bottom  = -(vp.col4() + vp.col2());
    frustum.left    = -(vp.col4() + vp.col3());

    frustum.zFar    = -(vp.col4() + vp.col1());
    frustum.top     = -(vp.col4() - vp.col2());
    frustum.right   = -(vp.col4() - vp.col3());

    vec4normalizeXYZ_remap(frustum.zNear);
    vec4normalizeXYZ_remap(frustum.zFar);
    vec4normalizeXYZ_remap(frustum.bottom);
    vec4normalizeXYZ_remap(frustum.top);
    vec4normalizeXYZ_remap(frustum.left);
    vec4normalizeXYZ_remap(frustum.right);*/

    bool heckYea = true;
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

bool Camera::withinFrustum(vec3 center, scalar radius) {
    vec3 nc = center;

    if (nc.dot(frustum.zNear.XYZ()) + frustum.zNear.w - radius > 0) {
        return false;
    }
    if (nc.dot(frustum.zFar.XYZ()) + frustum.zFar.w - radius > 0) {
        return false;
    }
    if (nc.dot(frustum.left.XYZ()) + frustum.left.w - radius > 0) {
        return false;
    }
    if (nc.dot(frustum.right.XYZ()) + frustum.right.w - radius > 0) {
        return false;
    }
    if (nc.dot(frustum.bottom.XYZ()) + frustum.bottom.w - radius > 0) {
        return false;
    }
    if (nc.dot(frustum.top.XYZ()) + frustum.top.w - radius > 0) {
        return false;
    }

    return true;
}