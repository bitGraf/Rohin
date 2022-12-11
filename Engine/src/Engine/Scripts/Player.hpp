#pragma once

#include "Engine.hpp"

namespace rh {
    class PlayerController : public rh::ScriptableBase {
    public:
        PlayerController() {}

        virtual void OnCreate() override {
            cWorld = &GetScene().GetCollisionWorld();
            LOG_ASSERT(cWorld, "PlayerController could not find Collision World");

            transformComponent = &GetComponent<TransformComponent>();
            colliderComponent = &GetComponent<ColliderComponent>();
            //auto followTarget = GetScene().FindByName("frog");

            LOG_ASSERT(transformComponent, "PlayerController could not find a transform component");
            LOG_ASSERT(colliderComponent, "PlayerController could not find a transform component");

            // Pos, Yaw, Pitch, Roll, Forward, Right, Up
            //math::Decompose(transformComponent->Transform, position);
            //math::Decompose(transformComponent->Transform, Forward, Right, Up);
            //math::Decompose(transformComponent->Transform, yaw, pitch);

            laml::transform::decompose(transformComponent->Transform, position, Forward, Right, Up, yaw, pitch, scale);

            hull_offset = laml::Vec3(0.0f, 0.5f, 0.0f);
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
            // handle rotation
            yaw -= Input::GetAxis("AxisRotateRight") * rotSpeed * ts;
        }

        laml::Vec3 GenerateDesiredMovement() {
            using namespace laml;

            if (Input::GetAxis("AxisBoost") > 0.0f) {
                moveSpeed = 20;
            }
            else {
                moveSpeed = 7.0f;
            }

            Vec3 vel(0.0f, 0.0f, 0.0f);
            if (grounded) {
                // get floor-based local frame
                Vec3 localU = normalize(m_floorUp);
                Vec3 localR = normalize(cross(Forward, localU));
                Vec3 localF = normalize(cross(localU, localR));

                // handle translation
                vel = vel + localR * (moveSpeed * Input::GetAxis("AxisMoveRight"));
                vel = vel + localF * (moveSpeed * Input::GetAxis("AxisMoveForward"));
                CameraHeight -= 0.05f * Input::GetAxis("AxisRotateUp");
                CameraHeight = std::clamp(CameraHeight, CameraHeightMin, CameraHeightMax);

                if (Input::GetAction("ActionJump") && grounded) {
                    vel = vel + Up * jumpPower; // still jump vertically, not off ramp
                    grounded = false;
                    m_floorUp = Vec3(0.0f, 1.0f, 0.0f);
                }
            }
            else {
                vel = velocity;
            }
            return vel;
        }

        laml::Vec3 GetCameraTarget() {
            float horiz_dist = sqrt(CameraDistance*CameraDistance - CameraHeight * CameraHeight);
            return position - (Forward * horiz_dist) + (Up * CameraHeight);
        }

        const laml::Vec3& GetPosition() const {
            return position;
        }

        virtual void OnUpdate(double ts) override {
            using namespace rh::laml;

            if (BeingControlled) {
                BENCHMARK_FUNCTION();

                velocity = GenerateDesiredMovement();
                RotateCharacter(ts);

                auto collisionHullID = colliderComponent->HullID;
                if (grounded) {
                    // Raycast down to see if there is anything beneath us
                    // TODO: Cache the currect collisionID of the ground and check against that.
                    RaycastResult rc = cWorld->Raycast(position + hull_offset, Vec3(0.0f, -1.0f, 0.0f), .6);

                    if (rc.colliderID == 0) {
                        // no ground beneath me
                        grounded = false;
                        m_floorUp = Vec3(0.0f, 1.0f, 0.0f);
                        m_floorID = 0;
                    }
                    else {
                        m_floorID = rc.colliderID;
                    }
                }
                else {
                    velocity.y -= 9.8 * ts;
                }

                int iterations = 0;
                // Perform collision logic
                if (collisionHullID > 0) {
                    // it has collision info
                    res = cWorld->Shapecast_multi(collisionHullID, velocity);

                    if (res.numContacts) {
                        Vec3 p_target = position + velocity * static_cast<float>(ts);
                        Vec3 p = p_target;
                        Vec3 p_start = position;
                        ghostPosition = position + (velocity*res.planes[0].TOI);

                        for (iterations = 0; iterations < 8; iterations++) {
                            float errorAcc = 0;
                            for (int n = 0; n < res.numContacts; n++) {
                                auto plane = &res.planes[n];

                                if (plane->TOI < ts) {
                                    Vec3 contactPoint = plane->contact_point;
                                    Vec3 contactNormal = plane->contact_normal;

                                    Vec3 p_t;
                                    if (n > 0) {
                                        p_t = p_start + velocity * (plane->TOI - res.planes[n - 1].TOI);
                                    }
                                    else {
                                        p_t = p_start + velocity * plane->TOI;
                                    }
                                    Vec3 body2contact = contactPoint - p_t;

                                    Vec3 sv = ((p + body2contact) - contactPoint);
                                    float s = dot((p + body2contact) - contactPoint, contactNormal);
                                    if (s < 0) {
                                        errorAcc -= s;

                                        p = p - (s - 1.0e-3f)*contactNormal;

                                        if (acos(dot(contactNormal, Vec3(0.0f, 1.0f, 0.0f))) < (m_floorAngleLimit * constants::deg2rad)) {
                                            grounded = true;
                                            velocity.y = 0;

                                            // this isn't the normal of the surface, 
                                            // so it might not be ideal for the floor normal
                                            m_floorUp = plane->contact_normal;
                                            m_floorID = plane->colliderID;
                                            // play sound?
                                            SoundEngine::CueSound("golem", position);
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
                        position = position + velocity * static_cast<float>(ts);
                        ghostPosition = position;
                    }

                    CollisionHull* hull = cWorld->getHullFromID(collisionHullID);
                    hull->position = position + hull_offset;

                    // TODO: Expand the ShapeCast function to allow for rotation
                    //hull->rotation.toYawPitchRoll(YawPitchRoll.x, 0, 0);
                }
                else {
                    // Just use normal movement.
                    position = position + velocity * static_cast<float>(ts);
                    ghostPosition = position;
                }

                if (grounded) {
                    velocity = Vec3(0.0f, 0.0f, 0.0f);
                }
                else {
                    //velocity = vec3(0, velocity.y, 0);
                }

                auto& transform = transformComponent->Transform;
                laml::transform::create_transform(transform, yaw, pitch, 0.0f, position, scale);
                laml::transform::decompose(transform, Forward, Right, Up);

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
        TransformComponent* transformComponent;
        ColliderComponent* colliderComponent;
        CollisionWorld* cWorld = nullptr;

        // Camera state
        //math::vec3 CameraTargetPosition;
        float CameraHeight = 2.5f;
        float CameraHeightMin = 1;
        float CameraHeightMax = 3.5f;
        float CameraDistance = 4;

        // State
        laml::Vec3 Forward, Right, Up;
        laml::Vec3 position;
        laml::Vec3 velocity;
        laml::Vec3 scale;
        float yaw, pitch;
        laml::Vec3 ghostPosition;
        laml::Vec3 m_floorUp;
        rh::UID_t m_floorID = 0;

        // Flags
        bool grounded = false;
        bool BeingControlled = true;

        // Parameters
        laml::Vec3 hull_offset;
        float moveSpeed = 7.0f;
        float jumpPower = 5.5f;
        float rotSpeed = 360.0f;
        float m_floorAngleLimit = 35;


        ShapecastResult_multi res;
    };

}