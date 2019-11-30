#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

#include "GameObject/Character.hpp"
#include "GameObject/Camera.hpp"
#include "Input.hpp"
#include "GLFW\glfw3.h"

/// A Character Object that responds to player input.
class PlayerObject : public CharacterObject {
public:
    PlayerObject();

    virtual void Update(double dt) override;
    virtual void PostLoad() override;

    virtual void InputEvent(Message::Datatype key, Message::Datatype action) override;

protected:
    Camera * m_Camera;

    bool CameraFollowPlayer;
};

#endif
