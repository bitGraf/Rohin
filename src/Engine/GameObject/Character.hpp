#ifndef CHARACTER_OBJECT_H
#define CHARACTER_OBJECT_H

#include "RenderableObject.hpp"

class CharacterObject : public RenderableObject {
public:
    CharacterObject();

    virtual void Update(double dt) override;
    virtual void Create(istringstream &iss, ResourceManager* resource) override;

    void MoveForward(float speed_t); //[-1,1]
    void MoveRight(float speed_t); //[-1,1]
    void Jump(float strength);

    vec3 Velocity;
    bool rotateToMovement;

protected:
    vec3 floorNormal;

    bool grounded;
    float speed;

    mat3 getLocalGroundTransform();
};

#endif
