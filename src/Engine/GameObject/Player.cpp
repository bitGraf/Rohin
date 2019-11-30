#include "Player.hpp"
#include "Scene\Scene.hpp"
const char* PlayerObject::_obj_type_PlayerObject = "Player";

PlayerObject::PlayerObject() :
    CameraFollowPlayer(false),
    m_controlType(eControlType::Normal)
{
    speed = 4;
    rotateSpeed = 90;
    rotateToMovement = false;
    m_relativeSource = eRelativeSource::Camera;
}

void PlayerObject::Update(double dt) {

    switch (m_controlType) {
        case eControlType::Normal: {
            if (Input::getKeyState("key_w")) {
                MoveForward(1);
            }
            if (Input::getKeyState("key_s")) {
                MoveForward(-1);
            }
            if (Input::getKeyState("key_a")) {
                MoveRight(-1);
            }
            if (Input::getKeyState("key_d")) {
                MoveRight(1);
            }
        } break;
        case eControlType::Tank: {
            if (Input::getKeyState("key_w")) {
                MoveForward(1);
            }
            if (Input::getKeyState("key_s")) {
                MoveForward(-1);
            }
            if (Input::getKeyState("key_a")) {
                Rotate(1);
            }
            if (Input::getKeyState("key_d")) {
                Rotate(-1);
            }
        } break;
    }

    CharacterObject::Update(dt);

    /* Move Camera behind Player */
    if (CameraFollowPlayer)
        m_cameraRef->lookAt(Position);
}

void PlayerObject::InputEvent(Message::Datatype key, Message::Datatype action) {
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        CameraFollowPlayer = !CameraFollowPlayer;
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        switch (m_relativeSource) {
            case eRelativeSource::World: {
                m_relativeSource = eRelativeSource::Camera;
            } break;
            case eRelativeSource::Camera: {
                m_relativeSource = eRelativeSource::Character;
            } break;
            case eRelativeSource::Character: {
                m_relativeSource = eRelativeSource::World;
            } break;
        }
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
        switch (m_controlType) {
            case eControlType::Normal: {
                m_controlType = eControlType::Tank;
                m_relativeSource = eRelativeSource::Character;
            } break;
            case eControlType::Tank: {
                m_controlType = eControlType::Normal;
            } break;
        }
    }
}

const char* PlayerObject::GetControlType() {
    switch (m_controlType) {
        case eControlType::Normal: {
            return "Normal Controls";
        } break;
        case eControlType::Tank: {
            return "Tank Controls";
        } break;
    }
}

void PlayerObject::PostLoad() {
    m_cameraRef = static_cast<Camera*>(GetScene()->getObjectByName("MainCamera"));
    Input::registerGameObject(this);
}

const char* PlayerObject::ObjectTypeString() {
    return _obj_type_PlayerObject;
}