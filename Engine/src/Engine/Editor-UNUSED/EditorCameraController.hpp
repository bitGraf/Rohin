/*
#pragma once

#include "Engine/GameObject/Components.hpp"
#include "Engine/Core/Input.hpp"
#include "Engine/Core/KeyCodes.hpp"

class EditorCameraController : public Engine::ScriptableBase {
public:
    EditorCameraController() = default;

    virtual void OnCreate() override {
        transformComponent = &GetComponent<Engine::TransformComponent>();
        transformComponent->Transform._14 = rand() % 10 - 5.0f;

        ENGINE_LOG_ASSERT(transformComponent, "CamController could not find a transform component");

        LOG_INFO("Camera controller created on GameObject {0}!", GetGameObjectID());
    }

    //virtual void OnDestroy() override {}

    virtual void OnUpdate(double ts) override {
        //auto& transform = GetComponent<Engine::TransformComponent>().Transform;
        auto& transform = transformComponent->Transform;

        if (Engine::Input::IsKeyPressed(KEY_CODE_A))
            transform._14 += speed * ts;
        if (Engine::Input::IsKeyPressed(KEY_CODE_D))
            transform._14 -= speed * ts;
        if (Engine::Input::IsKeyPressed(KEY_CODE_W))
            transform._24 -= speed * ts;
        if (Engine::Input::IsKeyPressed(KEY_CODE_S))
            transform._24 += speed * ts;
    }

private:
    Engine::TransformComponent* transformComponent;

    float speed = 5.0f;
};

*/