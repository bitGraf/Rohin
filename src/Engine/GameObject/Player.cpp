#include "Player.hpp"

PlayerObject::PlayerObject() {
    speed = 5;
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
}