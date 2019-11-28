#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

#include "GameObject/Character.hpp"
#include "Input.hpp"

class PlayerObject : public CharacterObject {
public:
    PlayerObject();

    virtual void Update(double dt) override;
};

#endif
