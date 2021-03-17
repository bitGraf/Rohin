#pragma once

#include "Engine.hpp"

class PlayerController : public Engine::ScriptableBase {
public:
    PlayerController() {}

    virtual void OnCreate() override {
        transformComponent = &GetComponent<Engine::TransformComponent>();
        //auto followTarget = GetScene().FindByName("frog");

        LOG_ASSERT(transformComponent, "CamController could not find a transform component");

        // Pos, Yaw, Pitch, Roll, Forward, Right, Up
        auto[_pos, _yaw, _pitch, _roll, _forward, _right, _up] = math::Decompose(transformComponent->Transform);
        position = _pos;
        yaw = _yaw;
        pitch = _pitch;
        Forward = _forward;
        Right = _right;
        Up = _up;

        ///TEMP
        yaw = 0;
        ///TEMP

        LOG_INFO("Player controller created on GameObject {0}!", GetGameObjectID());
    }

    void ToggleControl() {
        BeingControlled = !BeingControlled;
        if (BeingControlled)
            LOG_INFO("PlayerController now in control of the camera");
        else
            LOG_INFO("PlayerController no longer in control");
    }

    virtual void OnUpdate(double ts) override {

        if (BeingControlled) {
            if (Input::IsKeyPressed(KEY_CODE_Q)) {
                position -= Right * moveSpeed * ts; // Strafe Left
                updateTransform = true;
            } if (Input::IsKeyPressed(KEY_CODE_E)) {
                position += Right * moveSpeed * ts; // Strafe Right
                updateTransform = true;
            } if (Input::IsKeyPressed(KEY_CODE_W)) {
                position += Forward * moveSpeed * ts; // Walk Forward
                updateTransform = true;
            } if (Input::IsKeyPressed(KEY_CODE_S)) {
                position -= Forward * moveSpeed * ts; // Walk Backward
                updateTransform = true;
            } if (Input::IsKeyPressed(KEY_CODE_SPACE)) {
                position += Up * moveSpeed * ts; // Float up
                updateTransform = true;
            } if (Input::IsKeyPressed(KEY_CODE_LEFT_CONTROL)) {
                position -= Up * moveSpeed * ts; // Descend Down
                updateTransform = true;
            } if (Input::IsKeyPressed(KEY_CODE_A)) {
                yaw += rotSpeed * ts; // Rotate Left
                updateTransform = true;
            } if (Input::IsKeyPressed(KEY_CODE_D)) {
                yaw -= rotSpeed * ts; // Rotate Right
                updateTransform = true;
            } if (Input::IsKeyPressed(KEY_CODE_R)) {
                pitch += rotSpeed * ts; // Pitch Up
                updateTransform = true;
            } if (Input::IsKeyPressed(KEY_CODE_F)) {
                pitch -= rotSpeed * ts; // Pitch Down
                updateTransform = true;
            }

            if (Input::IsKeyPressed(KEY_CODE_4)) { // Reset transform
                yaw = 0;
                pitch = 0;
                updateTransform = true;
            }

            if (updateTransform) {
                auto& transform = transformComponent->Transform;

                transform = mat4();
                transform.translate(position);
                transform *= math::createYawPitchRollMatrix(yaw, 0.0f, pitch);
                auto[f, r, u] = math::GetUnitVectors(transform);
                Forward = f;
                Right = r;
                Up = u;
                updateTransform = false;
            }
        }
    }

private:
    Engine::TransformComponent * transformComponent;

    math::vec3 Forward, Right, Up;

    float moveSpeed = 4.0f;
    float rotSpeed = 360.0f;

    math::vec3 position;
    float yaw, pitch;
    bool updateTransform = true;
    bool BeingControlled = true;
};