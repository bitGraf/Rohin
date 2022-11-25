#pragma once

#include "Engine.hpp"

class Level : public rh::Scene {
public:
    Level(const std::string& levelName);
    ~Level();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(rh::Timestep ts) override;
    virtual void OnEvent(rh::Event& event) override;
    virtual void OnGuiRender() override;

private:
    void CheckViewportSize();
    bool OnKeyPressedEvent(rh::KeyPressedEvent& e);
    math::vec2 m_ViewportSize;

private:
    f32 m_levelTime = 0.0f;
    rh::Scene3D m_3DScene;
    rh::GameObject m_Camera;
    bool successful = false;
};