#pragma once

#include "Engine.hpp"

class AnimationScene : public Engine::Scene {
public:
    AnimationScene();
    virtual ~AnimationScene();

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

    Engine::md5::Model model;
};