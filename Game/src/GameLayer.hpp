#pragma once
#include <Engine.hpp>

class GameLayer {
public:
    GameLayer();
    virtual ~GameLayer() = default;

    virtual void OnAttach();
    virtual void OnDetach();

    virtual void OnUpdate(Engine::Timestep ts);
    virtual void OnEvent(Engine::Event& event);
    virtual void OnGuiRender();

private:
    void CheckViewportSize();
    bool OnKeyPressedEvent(Engine::KeyPressedEvent& e);

private:
    Engine::Ref<Engine::Scene3D> m_ActiveScene;

    Engine::GameObject m_Camera;

    math::vec2 m_ViewportSize;
};