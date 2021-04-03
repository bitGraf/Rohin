#include "MainMenu.hpp"

#include "Level.hpp"

MainMenuScene::MainMenuScene() {
    Name = "MainMenuScene";
}

MainMenuScene::~MainMenuScene() {

}


void MainMenuScene::OnAttach(){
    ENGINE_LOG_INFO("MainMenuScene Attach");

    m_ViewportSize = {
        (float)Engine::Application::Get().GetWindow().GetWidth(),
        (float)Engine::Application::Get().GetWindow().GetHeight() };
}

void MainMenuScene::OnDetach(){
    ENGINE_LOG_INFO("MainMenuScene Detach");
}

const std::vector<std::string> menuButtons = {
        "New Game",
        "Level Select",
        "Options",
        "Quit"
};
const std::vector<std::string> optionsButtons = {
        "Gameplay",
        "Input",
        "Sound",
        "Return"
};
const std::vector<std::string> levelSelectButtons = {
        "Level 1",
        "Level 2",
        "nbtTest",
        "Return"
};

const std::vector<std::string>& MainMenuScene::GetCurrentMenu() {
    switch (currentMenuState) {
    case 0: return menuButtons;
    case 1: return optionsButtons;
    case 2: return levelSelectButtons;
    }

    __debugbreak();
}

void MainMenuScene::OnUpdate(Engine::Timestep ts) {
    CheckViewportSize();

    // Setup Render
    Engine::RenderCommand::SetClearColor(math::vec4(.1, .1, .1, 1));
    Engine::RenderCommand::Clear();
    
    // Fill background
    int numX = 8;
    float ratio = m_ViewportSize.x / m_ViewportSize.y;
    Engine::SpriteRect src{ 0, 0, numX, numX / ratio};
    Engine::SpriteRect dst{ 0, 0, m_ViewportSize.x, m_ViewportSize.y};
    Engine::SpriteRenderer::SubmitSprite("Data/Images/grid/PNG/Dark/texture_07.png", &dst, &src);

    // draw title
    float x = m_ViewportSize.x / 2.0f;
    Engine::TextRenderer::SubmitText("font_big", "Bideo Gaem!", x, 35.0f, math::vec3(.5f, .25f, .7f), TextAlignment::ALIGN_TOP_MID);

    // draw menu buttons
    const std::vector<std::string>& currentMenuButtons = GetCurrentMenu();
    float y = m_ViewportSize.y * 0.25f;
    math::vec3 color(.6f, .8f, .75f);
    math::vec3 color_select(.1f, .8f, .75f);
    for (int n = 0; n < currentMenuButtons.size(); n++) {
        Engine::TextRenderer::SubmitText(currentMenuButtons.at(n), x, y, (n == currentSelection) ? color_select : color, TextAlignment::ALIGN_TOP_MID);
        y += 32;
    }

    // Draw other sprites
    auto[mx, my] = Engine::Input::GetMousePosition();
    src = { 0, 0, 1, 1 };
    dst = { mx, my, mx+16, my+16 };
    Engine::SpriteRenderer::SubmitSprite("Data/Images/frog.png", &dst, &src, ALIGN_MID_MID);
}

void MainMenuScene::OnEvent(Engine::Event& event) {
    // respond to input/game events
    Engine::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Engine::KeyPressedEvent>(BIND_EVENT_FN(MainMenuScene::OnKeyPressedEvent));

    dispatcher.Dispatch<Engine::WindowResizeEvent>([&](Engine::WindowResizeEvent& e) {
        m_ViewportSize = {
            (float)e.GetWidth(),
            (float)e.GetHeight()
        };

        return false;
    });
}

void MainMenuScene::GotoLevel(const std::string& levelName) {
    Level* newLevel = new Level(levelName);
    Engine::Application::Get().PushNewScene(newLevel);
}

bool MainMenuScene::OnKeyPressedEvent(Engine::KeyPressedEvent& e) {
    // quit game
    if (e.GetKeyCode() == KEY_CODE_ESCAPE) {
        Engine::Application::Get().Close();
    }

    auto& currentMenuButtons = GetCurrentMenu();
    if (e.GetKeyCode() == KEY_CODE_DOWN) {
        currentSelection++;
        if (currentSelection == currentMenuButtons.size())
            currentSelection = 0;
    }
    if (e.GetKeyCode() == KEY_CODE_UP) {
        if (currentSelection == 0)
            currentSelection = currentMenuButtons.size();
        currentSelection--;
    }

    if (e.GetKeyCode() == KEY_CODE_ENTER) {
        switch (currentMenuState) {
            case 0: {
                switch (currentSelection) {
                case 0: {GotoLevel("Level_1"); } break;
                case 1: {currentSelection = 0; currentMenuState = 2; } break;
                case 2: {currentSelection = 0; currentMenuState = 1; } break;
                case 3: {Engine::Application::Get().Close(); } break;
                }
            } break;
            case 1: {
                switch (currentSelection) {
                case 0: {ENGINE_LOG_WARN("Gameplay Options don't exist"); } break;
                case 1: {ENGINE_LOG_WARN("Input Options don't exist"); } break;
                case 2: {ENGINE_LOG_WARN("Sound Options don't exist"); } break;
                case 3: {currentMenuState = 0; currentSelection = 2; } break;
                }
            } break;
            case 2: {
                switch (currentSelection) {
                case 0: {GotoLevel("Level_1"); } break;
                case 1: {GotoLevel("Level_2"); } break;
                case 2: {GotoLevel("nbtTest"); } break;
                case 3: {currentMenuState = 0; currentSelection = 1; } break;
                }
            } break;
        }
    }

    return false;
}

void MainMenuScene::OnGuiRender() {
    // render debug ui
}

void MainMenuScene::CheckViewportSize() {
    static u32 specWidth = -1;
    static u32 specHeight = -1;

    if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
        (specWidth != m_ViewportSize.x || specHeight != m_ViewportSize.y)) {
        specWidth = (u32)m_ViewportSize.x;
        specHeight = (u32)m_ViewportSize.y;
        ENGINE_LOG_INFO("Render resolution: {0} x {1}", specWidth, specHeight);
        Engine::TextRenderer::OnWindowResize(specWidth, specHeight);
        Engine::SpriteRenderer::OnWindowResize(specWidth, specHeight);
    }
}