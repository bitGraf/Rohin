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

    if (!grounded) {
        Velocity.y -= 9.81 * dt;
    }
    else {
        // Raycast down to see if there is anything beneath us
        // TODO: Cache the currect collisionID of the ground and check against that.
        RaycastResult rc = cWorld.Raycast(Position + vec3(0, 0.5, 0), vec3(0, -1, 0), .6);

        if (rc.colliderID == 0) {
            grounded = false;
        }
    }

    // Perform collision logic
    if (m_collisionHullId > 0) {
        // it has collision info
        res = cWorld.Shapecast(m_collisionHullId, Velocity);

        if (res.numContacts) {
            vec3 p_target = Position + Velocity * dt;
            vec3 p = p_target;
            ghostPosition = Position + (Velocity*res.planes[0].TOI);

            for (int it = 0; it < 1; it++) {
                for (int n = 0; n < res.numContacts; n++) {
                    auto plane = &res.planes[n];

                    if (plane->TOI < dt) {
                        vec3 contactPoint = plane->contact_point;
                        vec3 contactNormal = plane->contact_normal;

                        vec3 p_t = Position + Velocity * plane->TOI;
                        vec3 body2contact = contactPoint - p_t;

                        vec3 sv = ((p + body2contact) - contactPoint);
                        float s = ((p + body2contact) - contactPoint).dot(contactNormal);
                        if (s < 0) {
                            p -= (s - 1.0e-3f)*contactNormal;

                            if (acos(contactNormal.dot(vec3(0, 1, 0))) < (45 * d2r)) {
                                grounded = true;
                                Velocity.y = 0;
                            }
                        }
                    }
                }
            }

            Position = p;
        }
        else {
            Position += Velocity * dt;
            ghostPosition = Position;
        }

        CollisionHull* hull = cWorld.getHullFromID(m_collisionHullId);
        hull->position = Position + vec3(0, 1, 0);

        // TODO: Expand the ShapeCast function to allow for rotation
        //hull->rotation.toYawPitchRoll(YawPitchRoll.x, 0, 0);
    }
    else {
        // Just use normal movement.
        Position += Velocity * dt;
        ghostPosition = Position;
    }

    Velocity = vec3(0, Velocity.y, 0);

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
        Velocity.y += strength;
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