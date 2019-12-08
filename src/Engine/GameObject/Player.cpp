#include "Player.hpp"
#include "Scene\Scene.hpp"
const char* PlayerObject::_obj_type_PlayerObject = "Player";

PlayerObject::PlayerObject() :
    m_controlType(eControlType::Normal)
{
    speed = 6;
    rotateSpeed = 360;
    rotateToMovement = true;
    m_relativeSource = eRelativeSource::Camera;
}

void PlayerObject::Update(double dt) {

    switch (m_controlType) {
        case eControlType::Normal: {
            MoveForward(Input::getAxisState("MoveForward"));
            MoveRight(Input::getAxisState("MoveRight"));
            Rotate(-Input::getAxisState("Rotate"));
        } break;
        case eControlType::Tank: {
            MoveForward(Input::getAxisState("MoveForward"));
            Rotate(-Input::getAxisState("MoveRight"));
        } break;
    }

    CharacterObject::Update(dt);


    // Reset character if they fall off the stage
    if (Position.y < -10) {
        Position = vec3(0, 2, 0);
        Velocity = vec3(0, 0, 0);
    }
}

void PlayerObject::Create(istringstream &iss, ResourceManager* resource) {
    CharacterObject::Create(iss, resource);
    Position = vec3(0, 2, 0);
    Velocity = vec3(0, 0, 0);
    m_collisionHullId = cWorld.CreateNewCapsule(resource, Position, 2, 0.5f);
}

void PlayerObject::InputEvent(Message::Datatype key, Message::Datatype action) {
    //if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    //    CameraFollowPlayer = !CameraFollowPlayer;
    //}

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

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        Jump(5.5);
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
    m_cameraID = GetScene()->getObjectIDByName("MainCamera"); // < Should this REALLY be in here?
    Input::registerGameObject(this);

    Input::watchKeyAxis("MoveForward", GLFW_KEY_W, GLFW_KEY_S);
    Input::watchKeyAxis("MoveRight", GLFW_KEY_D, GLFW_KEY_A);
    Input::watchKeyAxis("Rotate", GLFW_KEY_RIGHT, GLFW_KEY_LEFT);
}

const char* PlayerObject::ObjectTypeString() {
    return _obj_type_PlayerObject;
}