#include "Camera.hpp"
#include "Scene/Scene.hpp"
const char* Camera::_obj_type_CameraObject = "Camera";

Camera::Camera() :
    m_fovVert(60),
    m_zNear(.01),
    m_zFar(100),
    m_cameraMode(eCameraMode::Static),
    m_followYaw(0),
    m_followPitch(45),
    m_followDistance(14)
{}

void Camera::set(float fovVert, float z_near, float z_far) {
    m_fovVert = fovVert; //horizontal fov
    m_zNear = z_near;
    m_zFar = z_far;
}

void Camera::Create(jsonObj node) {
    GameObject::Create(node);

    auto fovHoriz = safeAccess<double>(node, "fovHoriz", 75);
    auto ne = safeAccess<double>(node, "near", .01);
    auto fa = safeAccess<double>(node, "far", 100);
    std::string followTarget = safeAccess<std::string>(node, "followTarget", "_followTarget_");

    //h = 2 * atan(AR*tan(v/2))
    //tan(h/2) = AR*tan(v/2)
    //v = 2 * atan(tan(h/2)/AR)
    f32 fovVert = 2 * atan(tan(fovHoriz * d2r/2.0) / (800.0/600.0)) * r2d;

    set(fovVert, ne, fa);
}

void Camera::Update(double dt) {

    switch (m_cameraMode) {
        case eCameraMode::Freefly: {
            vec3 vel;
            f32 rollChange = 0;

            f32 speed = 10;
            f32 rollRate = 60;

            f32 horizSens = 0.12; // Mouse based sensitivities
            f32 vertSens = 0.12;

			// These should be 1 2 3 but instead need to be 3 2 1 with the vel changes for fwd/back reversed
            vec3 forward = vec3(viewMatrix.row3()).get_unit();
            vec3 up = vec3(viewMatrix.row2()).get_unit();
            vec3 right = vec3(viewMatrix.row1()).get_unit();

            if (Input::getKeyState("key_w")) {
                vel -= forward;
            }
            if (Input::getKeyState("key_s")) {
                vel += forward;
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

            if (Input::getKeyState("key_numpad0")) {
                rollChange += 1;
            }
            if (Input::getKeyState("key_rctrl")) {
                rollChange -= 1;
            }

            Position += vel * speed * dt;
            YawPitchRoll.x += -Input::m_mouseMove.x * horizSens;
            YawPitchRoll.y += -Input::m_mouseMove.y * vertSens;
            YawPitchRoll.z += rollChange * rollRate * dt; // < Yaw and Pitch are already time scaled 
                                                          //   since they are captured by real mouse movement
        } break;
        case eCameraMode::Static: {
            return;
        } break;
        case eCameraMode::StaticLookAt: {
            //GameObject* followTarget = GetCurrentScene()->getObjectByID(m_followTargetID);
            //if (followTarget) {
            //    lookAt(followTarget->Position);
            //}
        } break;
        case eCameraMode::ThirdPersonFollow: {
            //GameObject* followTarget = GetCurrentScene()->getObjectByID(m_followTargetID);
            //if (followTarget) {
            //    CharacterObject* f = static_cast<CharacterObject*>(followTarget);
            //    vec3 forward = vec3(f->getTransform().col1());
            //    Position = followTarget->Position + vec3(0, 7, 0) - (forward.get_unit() * 8);
            //    lookAt(followTarget->Position);
            //}
            return;
        } break;
        case eCameraMode::OrbitFollow: {
            //GameObject* followTarget = GetCurrentScene()->getObjectByID(m_followTargetID);
            //if (followTarget) {
            //    if (Input::getLeftMouse()) {
            //        m_followYaw += (Input::m_mouseMove.x * .4);
            //    }
            //    if (Input::getLeftMouse()) {
            //        m_followPitch -= (Input::m_mouseMove.y * .08);
            //    }
            //
            //    vec3 camDir = vec3(-cos(m_followYaw*d2r)*cos(m_followPitch*d2r), sin(m_followPitch*d2r), -sin(m_followYaw*d2r)*cos(m_followPitch*d2r));
            //    Position = followTarget->Position + (camDir * m_followDistance);
            //    lookAt(followTarget->Position);
            //}
            return;
        } break;
    }
}

const char* Camera::ObjectTypeString() {
    return _obj_type_CameraObject;
}

void Camera::PostLoad() {
    //m_followTargetID = GetCurrentScene()->getObjectIDByName("YaBoy");
    Input::registerGameObject(this);

    originalPosition = Position;
    originalYPR = YawPitchRoll;
}

void Camera::updateViewFrustum(f32 width, f32 height) {
    updateViewMatrix();

    f32 tanHalf = tan(m_fovVert * d2r / 2);
    f32 m_aspectRatio = width / height;

    projectionMatrix = mat4(
        vec4(1 / (m_aspectRatio*tanHalf), 0, 0, 0),
        vec4(0, 1 / tanHalf, 0, 0),
        vec4(0, 0, -(m_zFar + m_zNear) / (m_zFar - m_zNear), -1),
        vec4(0, 0, -(2 * m_zFar*m_zNear) / (m_zFar - m_zNear), 0)
    );

    f32 vertFOV = m_fovVert * d2r; //radians
    f32 horizFOV = 2 * atan(m_aspectRatio*tanHalf); //radians

    f32 s_h = sin(horizFOV / 2);
    f32 c_h = cos(horizFOV / 2);
    f32 s_v = sin(vertFOV / 2);
    f32 c_v = cos(vertFOV / 2);

    mat4 vv = mat4(viewMatrix.row1(), viewMatrix.row2(), viewMatrix.row3(), viewMatrix.row4());

    frustum.top = vv * vec4(-s_v, c_v, 0, 0);
    frustum.bottom = vv * vec4(-s_v, -c_v, 0, 0);

    frustum.left = vv * vec4(-s_h, 0, -c_h, 0);
    frustum.right = vv * vec4(-s_h, 0, c_h, 0);

    frustum.zNear = vv * vec4(-1, 0, 0, m_zNear);
    frustum.zFar = vv * vec4(1, 0, 0, -m_zFar);
}

void Camera::updateViewMatrix() {
    viewMatrix = mat4(vec4(0, 0, -1, 0), vec4(0, 1, 0, 0), vec4(1, 0, 0, 0), vec4(0, 0, 0, 1)) * 
        math::createInverseTransform(Position, YawPitchRoll);

    /*if (m_parent) {
        mat4 parentTransform = mat4(vec4(0, 0, -1, 0), vec4(0, 1, 0, 0), vec4(1, 0, 0, 0), vec4(0, 0, 0, 1)) * 
            math::createInverseTransform(m_parent->Position, m_parent->YawPitchRoll);
        viewMatrix = viewMatrix * parentTransform;
    }*/
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

void Camera::InputEvent(s32 key, s32 action) {
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        switch (m_cameraMode) {
            case eCameraMode::Static: {
                m_cameraMode = eCameraMode::StaticLookAt;
            } break;
            case eCameraMode::StaticLookAt: {
                m_cameraMode = eCameraMode::ThirdPersonFollow;
            } break;
            case eCameraMode::ThirdPersonFollow: {
                m_cameraMode = eCameraMode::OrbitFollow;
            } break;
            case eCameraMode::OrbitFollow: {
                m_cameraMode = eCameraMode::Freefly;
            } break;
            case eCameraMode::Freefly: {
                m_cameraMode = eCameraMode::Static;

                Position = originalPosition;
                YawPitchRoll = originalYPR;
            } break;
        }
    }
}

void Camera::changeFollowTarget(UID_t target) {
	this->m_followTargetID = target;
}