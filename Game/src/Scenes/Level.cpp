#include "Level.hpp"

#include "MainMenu.hpp"

Level::Level(const std::string& levelName) {
    Name = levelName;
}

Level::~Level() {

}


void Level::OnAttach() {
    ENGINE_LOG_INFO("Level {0} Attach", Name);

    m_ViewportSize = {
        (float)Engine::Application::Get().GetWindow().GetWidth(),
        (float)Engine::Application::Get().GetWindow().GetHeight() };
}

void Level::OnDetach() {
    ENGINE_LOG_INFO("Level {0} Detach", Name);
}

void Level::OnUpdate(Engine::Timestep ts) {
    CheckViewportSize();

    // Setup Render
    Engine::RenderCommand::SetClearColor(math::vec4(.1, .1, .1, 1));
    Engine::RenderCommand::Clear();

    // draw level name
    if (m_levelTime < 5.0f) {
        Engine::TextRenderer::SubmitText("Level: " + Name, m_ViewportSize.x / 2.0f, 20.0f, math::vec3(.25f, .45f, .9f), TextAlignment::ALIGN_TOP_MID);
    }

    m_levelTime += ts;
}

void Level::OnEvent(Engine::Event& event) {
    // respond to input/game events
    Engine::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Engine::KeyPressedEvent>(BIND_EVENT_FN(Level::OnKeyPressedEvent));

    dispatcher.Dispatch<Engine::WindowResizeEvent>([&](Engine::WindowResizeEvent& e) {
        m_ViewportSize = {
            (float)e.GetWidth(),
            (float)e.GetHeight()
        };

        return false;
    });
}

bool Level::OnKeyPressedEvent(Engine::KeyPressedEvent& e) {
    // quit game
    if (e.GetKeyCode() == KEY_CODE_ESCAPE) {
        MainMenuScene* newScene = new MainMenuScene();
        Engine::Application::Get().PushNewScene(newScene);
    }

    return false;
}

void Level::OnGuiRender() {
    // render debug ui
}

void Level::CheckViewportSize() {
    static u32 specWidth = -1;
    static u32 specHeight = -1;

    if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
        (specWidth != m_ViewportSize.x || specHeight != m_ViewportSize.y)) {
        specWidth = (u32)m_ViewportSize.x;
        specHeight = (u32)m_ViewportSize.y;
        ENGINE_LOG_INFO("Render resolution: {0} x {1}", specWidth, specHeight);
        Engine::TextRenderer::OnWindowResize(specWidth, specHeight);
    }
}