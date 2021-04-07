#pragma once

#include "Engine.hpp"

class Gem : public Engine::ScriptableBase {
public:
    Gem() {}

    virtual void OnCreate() override {
        transformComponent = &GetComponent<Engine::TransformComponent>();

        LOG_ASSERT(transformComponent, "Gem script could not find a transform component");

        position = transformComponent->Transform.column4.asVec3();
        time = ((float)rand() / RAND_MAX) * 5.0f;

        LOG_INFO("Gem script created on GameObject {0}!", GetGameObjectID());
    }

    virtual void OnUpdate(double ts) override {
        if (m_static) return;

        time += ts;
        rotation += rotSpeed * ts;
        if (rotation > 360.0f)
            rotation -= 360.0f;
        position.y = 0.25f*(1+sin(time));

        auto& transform = transformComponent->Transform;
        math::CreateTransform(transform, position, rotation, 0);

        //auto& transform = transformComponent->Transform;
        //transform = mat4();
        //transform.translate(position);
        //transform *= math::createYawPitchRollMatrix(rotation, 0.0f, 0.0f);
        //transform *= mat4(scale, scale, scale, 1.0f);
    }

    void SetStatic() {
        m_static = true;
    }

    void SetDynamic() {
        m_static = true;
    }

private:
    Engine::TransformComponent * transformComponent;

    float time = 0.0f;
    float rotSpeed = 60.0f;

    math::vec3 position;
    float rotation = 0.0f;
    float scale = 0.25f;

    bool m_static = true;
};