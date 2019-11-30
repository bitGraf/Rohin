#include "Player.hpp"
#include "Scene\SceneManager.hpp"

PlayerObject::PlayerObject() :
    CameraFollowPlayer(false)
{
    speed = 4;
    rotateToMovement = true;
}

void PlayerObject::Update(double dt) {

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
    if (Input::getKeyState("key_space")) {
        Jump(1);
    }

    CharacterObject::Update(dt);

    /* Move Camera behind Player */
    if (CameraFollowPlayer)
        m_Camera->lookAt(Position);
}

void PlayerObject::InputEvent(Message::Datatype key, Message::Datatype action) {
    if (key == GLFW_KEY_F && action == GLFW_RELEASE) {
        CameraFollowPlayer = !CameraFollowPlayer;
    }
}

void PlayerObject::PostLoad() {
    m_Camera = static_cast<Camera*>(GetScene()->getObjectByName("MainCamera"));
    Input::registerInputEventCallback(this);
}