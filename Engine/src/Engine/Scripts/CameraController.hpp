#pragma once

#include "Engine/Sound/SoundEngine.hpp"
#include "Player.hpp"

namespace rh {
    class CameraController : public ScriptableBase {
    public:
        CameraController() {}

        /* When the NativeScriptComponent is creted at Scene3D::OnRuntimeStart(); */
        virtual void OnCreate() override {
            transformComponent = &GetComponent<TransformComponent>();
            cameraComponent = &GetComponent<CameraComponent>();
            //auto followTarget = GetScene().FindByName("frog");

            ENGINE_LOG_ASSERT(transformComponent, "CamController could not find a transform component");
            ENGINE_LOG_ASSERT(cameraComponent, "CamController could not find a camera component");

            cameraComponent->camera.SetOrthographic(6, -3, 3);
            cameraComponent->camera.SetPerspective(75.0f, 0.01f, 100.0f);

            // Pos, Yaw, Pitch, Roll, Forward, Right, Up
            laml::Vec3 scale;
            laml::transform::decompose(transformComponent->Transform, position, Forward, Right, Up, yaw, pitch, scale);
            //math::Decompose(transformComponent->Transform, position, Forward, Right, Up, yaw, pitch, scale);
            
            //math::Decompose(transformComponent->Transform, position);
            //math::Decompose(transformComponent->Transform, Forward, Right, Up);
            //math::Decompose(transformComponent->Transform, yaw, pitch);

            // update listener
            SoundEngine::SetListenerPosition(position);
            SoundEngine::SetListenerVelocity(laml::Vec3(0.0f, 0.0f, 0.0f));
            SoundEngine::SetListenerOrientation(Forward, Up);

            LOG_INFO("Camera controller created on GameObject {0}!", GetGameObjectID());
        }

        virtual void OnLink() override {
            playerScript = nullptr;

            auto player = GetScene().FindByName("Player");
            LOG_ASSERT(player, "CamController could not find player GameObject");

            playerScript = player.GetComponent<rh::NativeScriptComponent>().GetScript<PlayerController>();
            LOG_ASSERT(playerScript, "CamController could not find player controller script");

        }

        void ToggleControl() {
            BeingControlled = !BeingControlled;
            if (BeingControlled)
                LOG_INFO("CameraController now in control of the camera");
            else
                LOG_INFO("CameraController no longer in control");
        }

        virtual void OnUpdate(double ts) override {
            using namespace rh;
            using namespace laml;

            static bool firstFrame = true;
            static float oldMousePosX = 0, oldMousePosY = 0;
            float offX = 0.0f, offY = 0.0f;
            Vec3 velocity;

            if (BeingControlled) {
                BENCHMARK_FUNCTION();

                if (Input::IsMouseCaptured()) {
                    auto[newMousePosX, newMousePosY] = Input::GetMousePosition();

                    offX = newMousePosX - oldMousePosX;
                    offY = newMousePosY - oldMousePosY;

                    oldMousePosX = newMousePosX; // update old values
                    oldMousePosY = newMousePosY;
                }

                bool updateTransform = false;

                if (!firstFrame) {
                    if (offX != 0.0f) {
                        // mouse moved in x dir
                        yaw -= offX * 0.3f;
                        updateTransform = true;
                    }
                    if (offY != 0.0f) {
                        // mouse moved in y dir
                        pitch -= offY * 0.2f;
                        updateTransform = true;
                    }
                }
                firstFrame = false;
                if (!Input::IsMouseCaptured()) {
                    firstFrame = true;
                }

                if (Input::IsKeyPressed(KEY_CODE_LEFT_SHIFT)) {
                    moveSpeed = 20;
                }
                else {
                    moveSpeed = 5.0f;
                }

                if (Input::IsKeyPressed(KEY_CODE_A)) {
                    velocity = velocity - Right * moveSpeed;
                    updateTransform = true;
                } if (Input::IsKeyPressed(KEY_CODE_D)) {
                    velocity = velocity + Right * moveSpeed;
                    updateTransform = true;
                } if (Input::IsKeyPressed(KEY_CODE_W)) {
                    velocity = velocity + Forward * moveSpeed;
                    updateTransform = true;
                } if (Input::IsKeyPressed(KEY_CODE_S)) {
                    velocity = velocity - Forward * moveSpeed;
                    updateTransform = true;
                } if (Input::IsKeyPressed(KEY_CODE_SPACE)) {
                    velocity = velocity + Up * moveSpeed;
                    updateTransform = true;
                } if (Input::IsKeyPressed(KEY_CODE_LEFT_CONTROL)) {
                    velocity = velocity - Up * moveSpeed;
                    updateTransform = true;
                } if (Input::IsKeyPressed(KEY_CODE_Q)) {
                    yaw += rotSpeed * ts;
                    updateTransform = true;
                } if (Input::IsKeyPressed(KEY_CODE_E)) {
                    yaw -= rotSpeed * ts;
                    updateTransform = true;
                } if (Input::IsKeyPressed(KEY_CODE_R)) {
                    pitch += rotSpeed * ts;
                    updateTransform = true;
                } if (Input::IsKeyPressed(KEY_CODE_F)) {
                    pitch -= rotSpeed * ts;
                    updateTransform = true;
                }

                if (Input::IsKeyPressed(KEY_CODE_R) && Input::IsKeyPressed(KEY_CODE_LEFT_SHIFT)) {
                    yaw = 0;
                    pitch = 0;
                    updateTransform = true;
                }

                if (updateTransform) {
                    position = position + velocity * static_cast<float>(ts);
                    CalcTransformFromYawPitch(velocity);
                }
            }
            else if (playerScript) {
                firstFrame = true;
                oldMousePosX = 0;
                oldMousePosY = 0;
                Vec3 target = playerScript->GetCameraTarget();
                velocity = (target - position) * static_cast<float>(1.0 / ts);
                CalcTransformFromLookDir(target, playerScript->GetPosition(), velocity);
            }
        }

    private:
        void CalcTransformFromYawPitch(laml::Vec3 velocity) {
            using namespace rh;
            using namespace laml;

            auto& transform = transformComponent->Transform;
            laml::transform::create_transform(transform, yaw, pitch, 0.0f, position);
            laml::transform::decompose(transform, Forward, Right, Up);

            // update listener
            SoundEngine::SetListenerPosition(position);
            SoundEngine::SetListenerVelocity(velocity);
            SoundEngine::SetListenerOrientation(Forward, Up);
        }

        void CalcTransformFromLookDir(laml::Vec3 at, laml::Vec3 lookAt, laml::Vec3 velocity) {
            using namespace rh;
            using namespace laml;

            position = at;
            Forward = normalize(lookAt - at);
            Right = normalize(cross(Forward, Vec3(0.0f, 1.0f, 0.0f)));
            Up = normalize(cross(Right, Forward));
            auto& transform = transformComponent->Transform;

            laml::transform::create_transform_translate(transform, position);
            transform = laml::mul(transform, Mat4(Mat3(Right, Up, -Forward)));
            float _roll;
            laml::transform::decompose(transform, yaw, pitch, _roll);

            // update listener
            SoundEngine::SetListenerPosition(position);
            SoundEngine::SetListenerVelocity(velocity);
            SoundEngine::SetListenerOrientation(Forward, Up);
        }


    private:
        // Player
        PlayerController* playerScript;

        rh::TransformComponent* transformComponent;
        rh::CameraComponent* cameraComponent;

        laml::Vec3 Forward, Right, Up;

        float moveSpeed = 5.0f;
        float rotSpeed = 90.0f;

        laml::Vec3 position{ 0, 0, 2 };
        float yaw = 0, pitch = 0;
        //bool updateTransform = true;

        bool BeingControlled = false;
    };

}