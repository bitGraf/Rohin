#pragma once

#include "Engine.hpp"

class PlayerController : public Engine::ScriptableBase {
public:
    PlayerController() {}

    virtual void OnCreate() override {
        using namespace Engine;

        cWorld = &GetScene().GetCollisionWorld();
        LOG_ASSERT(cWorld, "PlayerController could not find Collision World");

        transformComponent = &GetComponent<TransformComponent>();
        colliderComponent  = &GetComponent<ColliderComponent>();
        //auto followTarget = GetScene().FindByName("frog");

        LOG_ASSERT(transformComponent, "PlayerController could not find a transform component");
        LOG_ASSERT(colliderComponent,  "PlayerController could not find a transform component");

        // Pos, Yaw, Pitch, Roll, Forward, Right, Up
        //math::Decompose(transformComponent->Transform, position);
        //math::Decompose(transformComponent->Transform, Forward, Right, Up);
        //math::Decompose(transformComponent->Transform, yaw, pitch);

        math::Decompose(transformComponent->Transform, position, Forward, Right, Up, yaw, pitch, scale);

        hull_offset = math::vec3(0, .5, 0);
        m_floorAngleLimit = 35;

        LOG_INFO("Player controller created on GameObject {0}!", GetGameObjectID());
    }

    void ToggleControl() {
        BeingControlled = !BeingControlled;
        if (BeingControlled)
            LOG_INFO("PlayerController now in control");
        else
            LOG_INFO("PlayerController no longer in control");
    }

    void RotateCharacter(double ts) {
        using namespace Engine;
        // handle rotation
        yaw -= Input::GetAxis("AxisRotateRight") * rotSpeed * ts;
    }

    math::vec3 GenerateDesiredMovement() {
        using namespace Engine;
        using namespace math;

        if (Input::GetAxis("AxisBoost") > 0.0f) {
            moveSpeed = 20;
        }
        else {
            moveSpeed = 7.0f;
        }

        vec3 vel(0,0,0);
        if (grounded) {
            // get floor-based local frame
            vec3 localU = m_floorUp.get_unit();
            vec3 localR = Forward.cross(localU).get_unit();
            vec3 localF = localU.cross(localR).get_unit();

            // handle translation
            vel += localR * (moveSpeed * Input::GetAxis("AxisMoveRight"));
            vel += localF * (moveSpeed * Input::GetAxis("AxisMoveForward"));
            CameraHeight -= 0.65f * Input::GetAxis("AxisRotateUp");
            CameraHeight = std::clamp(CameraHeight, CameraHeightMin, CameraHeightMax);

            if (Input::GetAction("ActionJump") && grounded) {
                vel += Up * jumpPower; // still jump vertically, not off ramp
                grounded = false;
                m_floorUp = vec3(0, 1, 0);
            }
        }
        else {
            vel = velocity;
        }
        return vel;
    }

    math::vec3 GetCameraTarget() {
        float horiz_dist = sqrt(CameraDistance*CameraDistance - CameraHeight * CameraHeight);
        return position - (Forward * horiz_dist) + (Up * CameraHeight);
    }

    const math::vec3& GetPosition() const {
        return position;
    }

    virtual void OnUpdate(double ts) override {
        using namespace Engine;
        using namespace math;

        if (BeingControlled) {
            BENCHMARK_FUNCTION();

            velocity = GenerateDesiredMovement();
            RotateCharacter(ts);

            auto collisionHullID = colliderComponent->HullID;
            if (grounded) {
                // Raycast down to see if there is anything beneath us
                // TODO: Cache the currect collisionID of the ground and check against that.
                RaycastResult rc = cWorld->Raycast(position + hull_offset, vec3(0, -1, 0), .6);

                if (rc.colliderID == 0) {
                    // no ground beneath me
                    grounded = false;
                    m_floorUp = vec3(0, 1, 0);
                    m_floorID = 0;
                }
                else {
                    m_floorID = rc.colliderID;
                }
            } else {
                velocity.y -= 9.8 * ts;
            }

            int iterations = 0;
            // Perform collision logic
            if (collisionHullID > 0) {
                // it has collision info
                res = cWorld->Shapecast_multi(collisionHullID, velocity);

                if (res.numContacts) {
                    vec3 p_target = position + velocity * ts;
                    vec3 p = p_target;
                    vec3 p_start = position;
                    ghostPosition = position + (velocity*res.planes[0].TOI);

                    for (iterations = 0; iterations < 8; iterations++) {
                        float errorAcc = 0;
                        for (int n = 0; n < res.numContacts; n++) {
                            auto plane = &res.planes[n];

                            if (plane->TOI < ts) {
                                vec3 contactPoint = plane->contact_point;
                                vec3 contactNormal = plane->contact_normal;

                                vec3 p_t;
                                if (n > 0) {
                                    p_t = p_start + velocity * (plane->TOI - res.planes[n - 1].TOI);
                                }
                                else {
                                    p_t = p_start + velocity * plane->TOI;
                                }
                                vec3 body2contact = contactPoint - p_t;

                                vec3 sv = ((p + body2contact) - contactPoint);
                                float s = ((p + body2contact) - contactPoint).dot(contactNormal);
                                if (s < 0) {
                                    errorAcc -= s;

                                    p -= (s - 1.0e-3f)*contactNormal;

                                    if (acos(contactNormal.dot(vec3(0, 1, 0))) < (m_floorAngleLimit * d2r)) {
                                        grounded = true;
                                        velocity.y = 0;

                                        // this isn't the normal of the surface, 
                                        // so it might not be ideal for the floor normal
                                        m_floorUp = plane->contact_normal;
                                        m_floorID = plane->colliderID;
                                        // play sound?
                                        //SoundEngine::CueSound("golem", position);
                                    }
                                }
                            }
                        }
                        float eps = 1e-9;
                        if (errorAcc < eps) {
                            // We went through en entire iteration without moving 
                            // the object, we can stop iterating now.
                            break;
                        }
                        p_start = p;
                    }

                    position = p;
                }
                else {
                    position += velocity * ts;
                    ghostPosition = position;
                }

                CollisionHull* hull = cWorld->getHullFromID(collisionHullID);
                hull->position = position + hull_offset;

                // TODO: Expand the ShapeCast function to allow for rotation
                //hull->rotation.toYawPitchRoll(YawPitchRoll.x, 0, 0);
            }
            else {
                // Just use normal movement.
                position += velocity * ts;
                ghostPosition = position;
            }

            if (grounded) {
                velocity = vec3(0, 0, 0);
            }
            else {
                //velocity = vec3(0, velocity.y, 0);
            }

            auto& transform = transformComponent->Transform;
            math::CreateTransform(transform, position, yaw, pitch);
            mat4 sc;
            math::CreateScale(sc, scale);
            transform = transform * sc;
            math::Decompose(transform, Forward, Right, Up);

            //transform = mat4();
            //transform.translate(position);
            //transform *= math::createYawPitchRollMatrix(yaw, 0.0f, pitch);
            //auto[f, r, u] = math::GetUnitVectors(transform);
            //Forward = f;
            //Right = r;
            //Up = u;
        }
    }

private:
    Engine::TransformComponent* transformComponent;
    Engine::ColliderComponent* colliderComponent;
    Engine::CollisionWorld* cWorld = nullptr;

    // Camera state
    //math::vec3 CameraTargetPosition;
    float CameraHeight = 2.5f;
    float CameraHeightMin = 1;
    float CameraHeightMax = 3.5f;
    float CameraDistance = 4;

    // State
    math::vec3 Forward, Right, Up;
    math::vec3 position;
    math::vec3 velocity;
    math::vec3 scale;
    float yaw, pitch;
    math::vec3 ghostPosition;
    math::vec3 m_floorUp;
    Engine::UID_t m_floorID = 0;

    // Flags
    bool grounded = false;
    bool BeingControlled = true;

    // Parameters
    math::vec3 hull_offset;
    float moveSpeed = 7.0f;
    float jumpPower = 5.5f;
    float rotSpeed = 360.0f;
    float m_floorAngleLimit = 35;


    Engine::ShapecastResult_multi res;
};