#ifndef CHARACTER_OBJECT_H
#define CHARACTER_OBJECT_H

#include "RenderableObject.hpp"

/// CharacterObject. Represents a Renderable Character that can move around.
class CharacterObject : public RenderableObject {
public:
    CharacterObject();

    virtual void Update(double dt) override;
    virtual void Create(istringstream &iss, ResourceManager* resource) override;
    virtual const char* ObjectTypeString() override;

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

private:
    static const char* _obj_type_CharacterObject;
};

#endif
