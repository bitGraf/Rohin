#include "Character.hpp"

CharacterObject::CharacterObject() :
    speed(1),
    grounded(false)
{}

void CharacterObject::Update(double dt) {
    // Transform into ground-space
    Velocity = getLocalGroundTransform() * Velocity;

    Position += Velocity * dt;

    Velocity = vec3();
}

void CharacterObject::Create(istringstream &iss, ResourceManager* resource) {
    RenderableObject::Create(iss, resource);

    floorNormal = vec3(0, 1, 0);
}

void CharacterObject::MoveForward(float speed_t) {
    vec3 direction = createYawPitchRoll_Forward(YawPitchRoll);

    Velocity += direction * speed_t * speed;
}

void CharacterObject::MoveRight(float speed_t) {
    vec3 direction = createYawPitchRoll_Right(YawPitchRoll);

    Velocity += direction * speed_t * speed;
}

void CharacterObject::Jump(float strength) {
}

mat3 CharacterObject::getLocalGroundTransform() {
    vec3 floorX = vec3(1, 0, 0);
    vec3 floorY = floorNormal;
    vec3 floorZ = floorX.cross(floorY);
    floorX = floorY.cross(floorZ);

    return mat3(
        floorX,
        floorY,
        floorZ
    );
}