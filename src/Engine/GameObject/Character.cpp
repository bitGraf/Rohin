#include "Character.hpp"
#include "Scene\Scene.hpp"
const char* CharacterObject::_obj_type_CharacterObject= "Character";

CharacterObject::CharacterObject() :
    speed(1),
    rotateSpeed(30),
    rotateToMovement(false),
    //m_cameraRef(nullptr),
    m_cameraID(0),
    m_relativeSource(eRelativeSource::World),
    m_collisionHullId(0)
{}

void CharacterObject::Update(double dt) {
    // Transform velocity into world-space
    Velocity = getRelativeAxes() * Velocity;

    if (Velocity.length_2() > 25) {
        auto bk = false;
    }

    if (Velocity.length() > 0.0) {
        // Perform collision logic
        if (m_collisionHullId > 0) {
            // it has collision info
            res = cWorld.Shapecast(m_collisionHullId, Velocity);
            ghostPosition = Position + (Velocity*res.TOI);

            if (res.TOI < 1) {
                // time of impact is TOI

                if (res.TOI < dt) {
                    vec3 contactNormal = -res.contact_normal; // get this from the Shapecast Result
                    // Check if we are moving away from the object
                    if (Velocity.dot(contactNormal) > 0) {
                        Position += Velocity * dt;
                    }
                    else {

                        // impact will happen between this frame and the next.

                        // Place character at point of impact
                        Position += (Velocity*res.TOI);

                        // Then substep the rest of the timestep.
                        vec3 pushOff = contactNormal * (0.5f);
                        vec3 tangentVelocity = Velocity - (Velocity.dot(contactNormal)*contactNormal);

                        Velocity = tangentVelocity + pushOff;
                        Position += Velocity * (dt - res.TOI);
                    }
                }
                else {
                    Position += Velocity * dt;
                }
            }
            else {
                // no collision imminent
                Position += Velocity * dt;
            }

            CollisionHull* hull = cWorld.getHullFromID(m_collisionHullId);
            hull->position = Position + vec3(0, 1, 0);
        }
        else {
            // Just use normal movement.
            Position += Velocity * dt;
            ghostPosition = Position;
        }
    }

    Velocity = vec3(0, 0, 0);
    if (Velocity.y < -0.1) {
        grounded = false;
    }

    if (m_relativeSource != eRelativeSource::Character && rotateToMovement) {
        vec3 direction = Velocity.get_unit();

        YawPitchRoll.x = atan2(-direction.z, direction.x) * r2d;
    }

    if (AngularVelocity.length_2() != 0.0) {
        YawPitchRoll += (AngularVelocity * dt);

        AngularVelocity = vec3();
    }
}

void CharacterObject::Create(istringstream &iss, ResourceManager* resource) {
    RenderableObject::Create(iss, resource);
}

void CharacterObject::PostLoad() {
}

const char* CharacterObject::ObjectTypeString() {
    return _obj_type_CharacterObject;
}

CollisionHull* CharacterObject::getHull() {
    if (m_collisionHullId > 0) {
        return cWorld.getHullFromID(m_collisionHullId);
    }
    else {
        return nullptr;
    }
}



void CharacterObject::MoveForward(float speed_t) {
    Velocity.x += speed_t * speed;
}

void CharacterObject::MoveRight(float speed_t) {
    Velocity.z += speed_t * speed;
}

void CharacterObject::Rotate(float speed_t) {

    AngularVelocity.x += (speed_t * rotateSpeed);
}

void CharacterObject::Jump(float strength) {
    if (grounded) {
        //Velocity.y += strength;
        grounded = false;
    }
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
            GameObject* camRef = GetScene()->getObjectByID(m_cameraID);
            if (camRef) {
                vec3 cameraX = vec3(camRef->getTransform().col1());
                vec3 cameraZ = vec3(camRef->getTransform().col3());

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