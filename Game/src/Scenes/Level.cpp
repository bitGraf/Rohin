#include "Level.hpp"

#include "MainMenu.hpp"

/* native scripts */
#include "../Scripts/CameraController.hpp"
#include "../Scripts/Player.hpp"

Level::Level(const std::string& levelName) {
    Name = levelName;
}

Level::~Level() {

}

bool Engine::BindGameScript(const std::string& script_tag, Engine::Scene3D* scene, GameObject gameobject) {
    if (script_tag.compare("script_camera_controller") == 0) {
        gameobject.AddComponent<Engine::NativeScriptComponent>().Bind<CameraController>(gameobject);
        return true;
    }
    if (script_tag.compare("script_player_controller") == 0) {
        gameobject.AddComponent<Engine::NativeScriptComponent>().Bind<PlayerController>(gameobject);
        return true;
    }
    return false;
}

void Level::OnAttach() {
    ENGINE_LOG_INFO("Level {0} Attach", Name);

    m_ViewportSize = {
        (float)Engine::Application::Get().GetWindow().GetWidth(),
        (float)Engine::Application::Get().GetWindow().GetHeight() };

    // Setup input bindings
    Engine::Input::CaptureMouse(false);
    Engine::Input::BindAxis("AxisMoveForward", { KEY_CODE_W, KEY_CODE_S, GAMEPAD_AXIS_LEFT_Y, true });
    Engine::Input::BindAxis("AxisMoveRight", { KEY_CODE_D, KEY_CODE_A, GAMEPAD_AXIS_LEFT_X });
    Engine::Input::BindAxis("AxisRotateRight", { KEY_CODE_E, KEY_CODE_Q, GAMEPAD_AXIS_RIGHT_X });
    Engine::Input::BindAxis("AxisRotateUp", { KEY_CODE_R, KEY_CODE_F, GAMEPAD_AXIS_RIGHT_Y, true });
    Engine::Input::BindAxis("AxisBoost", { KEY_CODE_LEFT_SHIFT, KEY_CODE_F12, GAMEPAD_AXIS_LEFT_TRIGGER });

    Engine::Input::BindAction("ActionJump", { KEY_CODE_SPACE, GAMEPAD_BUTTON_A });

    // load 3D scene data
    m_3DScene = std::make_shared<Engine::Scene3D>();

    if (!m_3DScene->loadFromFile(Name)) {
        ENGINE_LOG_ERROR("Could not find level named [{0}].", Name);
    }

    // Start the 3D scene
    m_3DScene->OnRuntimeStart();
}

void Level::OnDetach() {
    m_3DScene->Destroy();

    ENGINE_LOG_INFO("Level {0} Detach", Name);
}

void Level::OnUpdate(Engine::Timestep ts) {
    CheckViewportSize();

    // Setup Render
    Engine::RenderCommand::SetClearColor(math::vec4(.1, .1, .1, 1));
    Engine::RenderCommand::Clear();

    // draw 3D scene
    m_3DScene->OnUpdate(ts);

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
        return true;
    }

    // toggle mouse controll for camera
    if (e.GetKeyCode() == KEY_CODE_TAB) {
        if (Engine::Input::IsMouseCaptured())
            Engine::Input::CaptureMouse(false);
        else
            Engine::Input::CaptureMouse(true);
        return true;
    }

    if (e.GetKeyCode() == KEY_CODE_C) {
        if (m_Camera) {
            if (m_Camera.HasComponent<Engine::NativeScriptComponent>()) {
                auto scriptComp = m_Camera.GetComponent<Engine::NativeScriptComponent>();
                auto script = scriptComp.GetScript<CameraController>();
                script->ToggleControl();
            }
        }

        auto player = m_3DScene->FindByName("Player");
        if (player) {
            auto script = player.GetComponent<Engine::NativeScriptComponent>().GetScript<PlayerController>();
            script->ToggleControl();
        }

        return true;
    }

    if (e.GetKeyCode() == KEY_CODE_O) {
        Engine::Renderer::NextOutputMode();
        return true;
    }
    if (e.GetKeyCode() == KEY_CODE_T) {
        Engine::Renderer::ToggleToneMapping();
        return true;
    }
    if (e.GetKeyCode() == KEY_CODE_G) {
        Engine::Renderer::ToggleGammaCorrection();
        return true;
    }
    if (e.GetKeyCode() == KEY_CODE_3) {
        m_3DScene->ToggleCollisionHulls();
        return true;
    }

    if (e.GetKeyCode() == KEY_CODE_M) {
        Engine::Renderer::ToggleDebugSoundOutput();
        return true;
    }
    if (e.GetKeyCode() == KEY_CODE_J) {
        Engine::Renderer::ToggleDebugControllerOutput();
        return true;
    }
    if (e.GetKeyCode() == KEY_CODE_BACKSLASH) {
        Engine::Renderer::RecompileShaders();
        return true;
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
        Engine::Renderer::OnWindowResize(specWidth, specHeight);
        m_3DScene->OnViewportResize(specWidth, specHeight);
    }
}