#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

#include "GameObject/Character.hpp"
#include "GameObject/Camera.hpp"
#include "Input.hpp"
#include "GLFW/glfw3.h"

/// A Character Object that responds to player input.
class PlayerObject : public CharacterObject {
public:
    PlayerObject();

    virtual void Update(double dt) override;
    virtual void PostLoad() override;
    virtual void Create(istringstream &iss) override;

    virtual void InputEvent(s32 key, s32 action) override;
    virtual const char* ObjectTypeString() override;

    const char* GetControlType();

	bool activeState = false;

protected:
    enum class eControlType {
        Normal,
        Tank
    };
    eControlType m_controlType;

private:
    static const char* _obj_type_PlayerObject;
	void switchActivePlayer();
};

#endif
