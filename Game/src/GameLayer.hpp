#pragma once
#include <Engine.hpp>

class GameLayer : public Engine::EngineLayer {
public:
    GameLayer();
    virtual ~GameLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    virtual void OnUpdate(Engine::Timestep ts) override;
    virtual void OnEvent(Engine::Event& event) override;
    virtual void OnGuiRender() override;

    virtual Engine::Ref<Engine::Scene> GetScene() override { return m_ActiveScene; }
    virtual Engine::Ref<Engine::Framebuffer> GetOutput() override { return m_Framebuffer; }

private:
    void CheckViewportSize();
    bool OnKeyPressedEvent(Engine::KeyPressedEvent& e);

private:
    Engine::Ref<Engine::Scene> m_ActiveScene;
    Engine::Ref<Engine::Framebuffer> m_Framebuffer;

    Engine::GameObject m_Camera;
    
    math::vec2 m_ViewportSize;
};