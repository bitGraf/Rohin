#include "Character.hpp"
const char* CharacterObject::_obj_type_CharacterObject= "Character";

CharacterObject::CharacterObject() :
    speed(1),
    rotateSpeed(30),
    rotateToMovement(false),
    m_cameraRef(nullptr),
    m_relativeSource(eRelativeSource::World)
{}

void CharacterObject::Update(double dt) {
    if (Velocity.length_2() != 0.0) {

        // Transform into ground-space
        Velocity = getRelativeAxes() * Velocity;

        Position += Velocity * dt;

        if (rotateToMovement) {
            vec3 direction = Velocity.get_unit();

            mesh_YawPitchRoll.x = atan2(-direction.z, direction.x) * r2d;
        }

        Velocity = vec3();
    }

    if (AngularVelocity.length_2() != 0.0) {
        YawPitchRoll += (AngularVelocity * dt);

        AngularVelocity = vec3();
    }
}

void CharacterObject::Create(istringstream &iss, ResourceManager* resource) {
    RenderableObject::Create(iss, resource);
}

const char* CharacterObject::ObjectTypeString() {
    return _obj_type_CharacterObject;
}



void CharacterObject::MoveForward(float speed_t) {
    vec3 direction = vec3(1, 0, 0);// createYawPitchRoll_Forward(YawPitchRoll);

    Velocity += direction * speed_t * speed;
}

void CharacterObject::MoveRight(float speed_t) {
    vec3 direction = vec3(0, 0, 1);// createYawPitchRoll_Right(YawPitchRoll);

    Velocity += direction * speed_t * speed;
}

void CharacterObject::Rotate(float speed_t) {

    AngularVelocity.x += (speed_t * rotateSpeed);
}

void CharacterObject::Jump(float strength) {
}

const char* CharacterObject::GetRelativeMovementType() {
    switch (m_relativeSource) {
    case eRelativeSource::World: {
        return "World";
    } break;
    case eRelativeSource::Camera: {
        return "Camera";
    } break;
    case eRelativeSource::Character: {
        return "Character";
    } break;
    }
}

mat3 CharacterObject::getRelativeAxes() {

    switch (m_relativeSource) {
        case eRelativeSource::World: {
            return mat3();
        } break;
        case eRelativeSource::Camera: {
            if (m_cameraRef) {
                vec3 cameraX = vec3(m_cameraRef->getTransform().col1());
                vec3 cameraZ = vec3(m_cameraRef->getTransform().col3());

                cameraX.y = 0;
                cameraZ.y = 0;

                cameraX.normalize();
                cameraZ.normalize();

                return mat3(
                    cameraX,
                    vec3(0, 1, 0),
                    cameraZ
                );
            }
            else {
                return mat3();
            }
        } break;
        case eRelativeSource::Character: {
            vec3 localX = math::createYawPitchRoll_Forward(YawPitchRoll);
            vec3 localZ = math::createYawPitchRoll_Right(YawPitchRoll);

            localX.y = 0;
            localZ.y = 0;

            localX.normalize();
            localZ.normalize();

            return mat3(
                localX,
                vec3(0,1,0),
                localZ
            );
        } break;
    }
}