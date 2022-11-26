#pragma once

#include "Engine.hpp"

class Gem : public rh::ScriptableBase {
public:
    Gem() {}

    virtual void OnCreate() override {
        transformComponent = &GetComponent<rh::TransformComponent>();

        LOG_ASSERT(transformComponent, "Gem script could not find a transform component");

        position = rh::laml::Vec3(transformComponent->Transform.c_14, transformComponent->Transform.c_24, transformComponent->Transform.c_34);
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
        rh::laml::transform::create_transform(transform, rotation, 0.0f, 0.0f, position);

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
    rh::TransformComponent * transformComponent;

    float time = 0.0f;
    float rotSpeed = 60.0f;

    rh::laml::Vec3 position;
    float rotation = 0.0f;
    float scale = 0.25f;

    bool m_static = true;
};