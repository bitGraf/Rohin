#pragma once

#include "Engine.hpp"

class MainMenuScene : public Engine::Scene {
public:
    MainMenuScene();
    virtual ~MainMenuScene();

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
    void GotoLevel(const std::string& levelName);

    const std::vector<std::string>& GetCurrentMenu();
    u8 currentSelection = 0;
    u8 currentMenuState = 0;
};