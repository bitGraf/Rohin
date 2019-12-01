#include "Camera.hpp"
const char* Camera::_obj_type_CameraObject = "Camera";

Camera::Camera() :
    m_fov(75),
    m_zNear(.01),
    m_zFar(100),
    freeFlyMode(false)
{
    m_type = GameObjectType::Camera;
}

void Camera::set(float fov, float z_near, float z_far) {
    m_fov = fov;
    m_zNear = z_near;
    m_zFar = z_far;
}

void Camera::Create(istringstream &iss, ResourceManager* resource) {
    GameObject::Create(iss, resource);

    auto fov = getNextFloat(iss);
    auto ne = getNextFloat(iss);
    auto fa = getNextFloat(iss);

    set(fov, ne, fa);
}

const char* Camera::ObjectTypeString() {
    return _obj_type_CameraObject;
}

void Camera::updateViewFrustum(f32 width, f32 height) {
    updateViewMatrix();

    f32 tanHalf = tan(m_fov * d2r / 2);
    f32 m_aspectRatio = width / height;

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

    frustum.zNear = vv * vec4(-1, 0, 0, m_zNear);
    frustum.zFar = vv * vec4(1, 0, 0, -m_zFar);
}

void Camera::updateViewMatrix() {
    viewMatrix = math::createInverseTransform(Position, YawPitchRoll);

    if (m_parent) {
        mat4 parentTransform = math::createInverseTransform(m_parent->Position, m_parent->YawPitchRoll);
        viewMatrix = viewMatrix * parentTransform;
    }
}

/*void Camera::updateProjectionMatrix(f32 width, f32 height) {
    f32 tanHalf = tan(m_fov * d2r / 2);
    m_aspectRatio = width / height;

    projectionMatrix = mat4(
        vec4(1 / (m_aspectRatio*tanHalf), 0, 0, 0),
        vec4(0, 1 / tanHalf, 0, 0),
        vec4(0, 0, -(m_zFar + m_zNear) / (m_zFar - m_zNear), -1),
        vec4(0, 0, -(2 * m_zFar*m_zNear) / (m_zFar - m_zNear), 0)
    ) * mat4(vec4(0, 0, -1, 0), vec4(0, 1, 0, 0), vec4(1, 0, 0, 0), vec4(0, 0, 0, 1));
}*/

Camera& Camera::lookAt(vec3 target, bool updateMatrix) {
    vec3 forward = (target - Position).get_unit();  // +X
    vec3 right;
    if (abs(forward.dot(vec3(0, 1, 0))) > 0.99) {
        right = forward.cross(vec3(0, 0, 1));      // +Z
    }
    else {
        right = forward.cross(vec3(0, 1, 0));      // +Z
    }
    vec3 up = right.cross(forward);                 // +Y

    YawPitchRoll.y = asin(forward.y) * r2d;
    YawPitchRoll.x = -atan2(forward.z, forward.x) * r2d;
    YawPitchRoll.z = -atan2(right.y, up.y) * r2d;

    if (YawPitchRoll.y > 80) {
        YawPitchRoll.y = 80;
    }
    if (YawPitchRoll.y < -80) {
        YawPitchRoll.y = -80;
    }

    if (updateMatrix) updateViewMatrix();

    return *this;
}

bool Camera::withinFrustum(vec3 location, float radius) {

    if (location.dot(frustum.zNear.XYZ()) + frustum.zNear.w - radius > 0) {
        return false;
    }
    if (location.dot(frustum.zFar.XYZ()) + frustum.zFar.w - radius > 0) {
        return false;
    }
    if (location.dot(frustum.left.XYZ()) + frustum.left.w - radius > 0) {
        return false;
    }
    if (location.dot(frustum.right.XYZ()) + frustum.right.w - radius > 0) {
        return false;
    }
    if (location.dot(frustum.bottom.XYZ()) + frustum.bottom.w - radius > 0) {
        return false;
    }
    if (location.dot(frustum.top.XYZ()) + frustum.top.w - radius > 0) {
        return false;
    }

    return true;
}

void Camera::Update(double dt) {
    if (freeFlyMode) {
        vec3 vel;
        f32 yawChange = 0;
        f32 pitchChange = 0;
        f32 rollChange = 0;

        f32 speed = 10;
        f32 yawRate = 60;
        f32 pitchRate = 60;
        f32 rollRate = 60;

        f32 horizSens = 0.002;
        f32 vertSens = 0.002;

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

        Position += vel * dt;
        YawPitchRoll.x += yawChange;
        YawPitchRoll.y += pitchChange;
        YawPitchRoll.z += rollChange;
    }
}