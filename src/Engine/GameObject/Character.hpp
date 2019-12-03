#ifndef CHARACTER_OBJECT_H
#define CHARACTER_OBJECT_H

#include "RenderableObject.hpp"
#include "Camera.hpp"

/// CharacterObject. Represents a Renderable Character that can move around.
class CharacterObject : public RenderableObject {
public:
    CharacterObject();

    virtual void Update(double dt) override;
    virtual void Create(istringstream &iss, ResourceManager* resource) override;
    virtual const char* ObjectTypeString() override;

    const char* GetRelativeMovementType();

    void MoveForward(float speed_t); //[-1,1]
    void MoveRight(float speed_t); //[-1,1]
    void Rotate(float speed_t); //[-1,1]
    void Jump(float strength);

    vec3 Velocity;
    vec3 AngularVelocity;
    bool rotateToMovement;

protected:
    enum class eRelativeSource { // How the input is mapped to movement
        World,  // Based on Global XYZ
        Camera, // Based on screen-space Left/Right/Forward/Backward
        Character // Based on Character's local XYZ
    };
    eRelativeSource m_relativeSource;
    UID_t m_cameraID;

    float speed;
    float rotateSpeed;

    mat3 getRelativeAxes();

private:
    static const char* _obj_type_CharacterObject;
};

#endif
