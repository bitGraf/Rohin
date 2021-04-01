#pragma once

#include "Engine.hpp"

class Level : public Engine::Scene {
public:
    Level(const std::string& levelName);
    ~Level();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(Engine::Timestep ts) override;
    virtual void OnEvent(Engine::Event& event) override;
    virtual void OnGuiRender() override;

private:
    void CheckViewportSize();
    bool OnKeyPressedEvent(Engine::KeyPressedEvent& e);
    math::vec2 m_ViewportSize;

private:
    f32 m_levelTime = 0.0f;
};