#pragma once

#include "Engine.hpp"

using namespace Engine;
using namespace math;
class CameraController : public Engine::ScriptableBase {
public:
    CameraController() {}

    virtual void OnCreate() override {
        transformComponent = &GetComponent<TransformComponent>();
        cameraComponent = &GetComponent<CameraComponent>();
        //auto followTarget = GetScene().FindByName("frog");

        LOG_ASSERT(transformComponent, "CamController could not find a transform component");
        LOG_ASSERT(cameraComponent,    "CamController could not find a camera component");

        cameraComponent->camera.SetOrthographic(6, -3, 3);
        cameraComponent->camera.SetPerspective(75.0f, 0.01f, 100.0f);

        // Pos, Yaw, Pitch, Roll, Forward, Right, Up
        auto [_pos, _yaw, _pitch, _roll, _forward, _right, _up] = math::Decompose(transformComponent->Transform);
        position = _pos;
        yaw = _yaw;
        pitch = _pitch;
        Forward = _forward;
        Right = _right;
        Up = _up;

        ///TEMP
        yaw = 0;
        ///TEMP

        LOG_INFO("Camera controller created on GameObject {0}!", GetGameObjectID());
    }

    void ToggleControl() {
        BeingControlled = !BeingControlled;
        if (BeingControlled)
            LOG_INFO("CameraController now in control of the camera");
        else
            LOG_INFO("CameraController no longer in control");
    }

    virtual void OnUpdate(double ts) override {
        static bool firstFrame = true;
        static float oldMousePosX = 0, oldMousePosY = 0;
        float offX = 0.0f, offY = 0.0f;

        if (BeingControlled) {
            if (Input::IsMouseCaptured()) {
                auto[newMousePosX, newMousePosY] = Input::GetMousePosition();

                offX = newMousePosX - oldMousePosX;
                offY = newMousePosY - oldMousePosY;

                oldMousePosX = newMousePosX; // update old values
                oldMousePosY = newMousePosY;
            }

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

            vec3 velocity;
            if (Input::IsKeyPressed(KEY_CODE_A)) {
                velocity -= Right * moveSpeed;
                updateTransform = true;
            } if (Input::IsKeyPressed(KEY_CODE_D)) {
                velocity += Right * moveSpeed;
                updateTransform = true;
            } if (Input::IsKeyPressed(KEY_CODE_W)) {
                velocity += Forward * moveSpeed;
                updateTransform = true;
            } if (Input::IsKeyPressed(KEY_CODE_S)) {
                velocity -= Forward * moveSpeed;
                updateTransform = true;
            } if (Input::IsKeyPressed(KEY_CODE_SPACE)) {
                velocity += Up * moveSpeed;
                updateTransform = true;
            } if (Input::IsKeyPressed(KEY_CODE_LEFT_CONTROL)) {
                velocity -= Up * moveSpeed ;
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

            if (Input::IsKeyPressed(KEY_CODE_R)) {
                yaw = 0;
                pitch = 0;
                updateTransform = true;
            }

            if (updateTransform) {
                auto& transform = transformComponent->Transform;
                position += velocity * ts;

                transform = mat4();
                transform.translate(position);
                transform *= math::createYawPitchRollMatrix(yaw, 0.0f, pitch);
                auto[f, r, u] = math::GetUnitVectors(transform);
                Forward = f;
                Right = r;
                Up = u;
                updateTransform = false;

                // update listener
                SoundEngine::SetListenerPosition(position);
                SoundEngine::SetListenerVelocity(velocity);
                SoundEngine::SetListenerOrientation(Forward, Up);
            }
        } else {
            firstFrame = true;
        }
    }

private:
    TransformComponent* transformComponent;
    CameraComponent* cameraComponent;

    vec3 Forward, Right, Up;

    float moveSpeed = 5.0f;
    float rotSpeed = 90.0f;

    vec3 position{0, 0, 2};
    float yaw = 0, pitch = 0;
    bool updateTransform = true;

    bool BeingControlled = false;
};