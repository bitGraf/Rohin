#include "Level.hpp"

#include "MainMenu.hpp"

/* native scripts */
#include "Engine/Scripts/CameraController.hpp"
#include "Engine/Scripts/Player.hpp"

Level::Level(const std::string& levelName) {
    Name = levelName;
}

Level::~Level() {

}

void Level::OnAttach() {
    ENGINE_LOG_INFO("Level {0} Attach", Name);

    m_ViewportSize = {
        (float)rh::Application::Get().GetWindow().GetWidth(),
        (float)rh::Application::Get().GetWindow().GetHeight() };

    // Setup input bindings
    rh::Input::CaptureMouse(false);
    rh::Input::BindAxis("AxisMoveForward", { KEY_CODE_W, KEY_CODE_S, GAMEPAD_AXIS_LEFT_Y, true });
    rh::Input::BindAxis("AxisMoveRight", { KEY_CODE_D, KEY_CODE_A, GAMEPAD_AXIS_LEFT_X });
    rh::Input::BindAxis("AxisRotateRight", { KEY_CODE_E, KEY_CODE_Q, GAMEPAD_AXIS_RIGHT_X });
    rh::Input::BindAxis("AxisRotateUp", { KEY_CODE_R, KEY_CODE_F, GAMEPAD_AXIS_RIGHT_Y, true });
    rh::Input::BindAxis("AxisBoost", { KEY_CODE_LEFT_SHIFT, KEY_CODE_F12, GAMEPAD_AXIS_LEFT_TRIGGER });

    rh::Input::BindAction("ActionJump", { KEY_CODE_SPACE, GAMEPAD_BUTTON_A });

    // load 3D scene data
    if (!m_3DScene.loadFromLevel(Name)) {
        ENGINE_LOG_ERROR("Could not find level named [{0}].", Name);
    }

    // Start the 3D scene
    m_3DScene.OnRuntimeStart();
    m_levelTime = 0.0f;
}

void Level::OnDetach() {
    m_3DScene.Destroy();

    ENGINE_LOG_INFO("Level {0} Detach", Name);
}

void Level::OnUpdate(rh::Timestep ts) {
    CheckViewportSize();

    // Setup Render
    rh::RenderCommand::SetClearColor(rh::laml::Vec4(.1, .1, .1, 1));
    rh::RenderCommand::Clear();

    // draw 3D scene
    m_3DScene.OnUpdate(ts);

    // draw level name
    if (m_levelTime < 5.0f) {
        rh::TextRenderer::SubmitText("Level: " + Name, m_ViewportSize.x / 2.0f, 20.0f, rh::laml::Vec3(.25f, .45f, .9f), rh::TextAlignment::ALIGN_TOP_MID);
    }

    m_levelTime += ts;
}

void Level::OnEvent(rh::Event& event) {
    // respond to input/game events
    rh::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<rh::KeyPressedEvent>(BIND_EVENT_FN(Level::OnKeyPressedEvent));

    dispatcher.Dispatch<rh::WindowResizeEvent>([&](rh::WindowResizeEvent& e) {
        m_ViewportSize = {
            (float)e.GetWidth(),
            (float)e.GetHeight()
        };

        return false;
    });
}

bool Level::OnKeyPressedEvent(rh::KeyPressedEvent& e) {
    // quit game
    if (e.GetKeyCode() == KEY_CODE_ESCAPE) {
        MainMenuScene* newScene = new MainMenuScene();
        rh::Application::Get().PushNewScene(newScene);
        return true;
    }

    // toggle mouse controll for camera
    if (e.GetKeyCode() == KEY_CODE_TAB) {
        if (rh::Input::IsMouseCaptured())
            rh::Input::CaptureMouse(false);
        else
            rh::Input::CaptureMouse(true);
        return true;
    }

    if (e.GetKeyCode() == KEY_CODE_C) {
        auto camera = m_3DScene.FindByName("Camera");
        if (camera) {
            if (camera.HasComponent<rh::NativeScriptComponent>()) {
                auto scriptComp = camera.GetComponent<rh::NativeScriptComponent>();
                auto script = scriptComp.GetScript<rh::CameraController>();
                script->ToggleControl();
            }
        }

        auto player = m_3DScene.FindByName("Player");
        if (player) {
            auto script = player.GetComponent<rh::NativeScriptComponent>().GetScript<rh::PlayerController>();
            script->ToggleControl();
        }

        return true;
    }

    if (e.GetKeyCode() == KEY_CODE_SPACE) {
        auto player = m_3DScene.FindByName("Player");
        if (player) {
            auto script = player.GetComponent<rh::NativeScriptComponent>().GetScript<rh::PlayerController>();
            auto pos_script = script->GetPosition();
            auto transform = player.GetComponent<rh::TransformComponent>().Transform;
            auto pos_trans = rh::laml::Vec3(transform.c_14, transform.c_24, transform.c_34);

            LOG_DEBUG("Player scipt position: {0}", pos_script);
            LOG_DEBUG("Player trans position: {0}", pos_trans);
        }
        return true;
    }

    if (e.GetKeyCode() == KEY_CODE_O) {
        rh::Renderer::NextOutputMode();
        return true;
    }
    if (e.GetKeyCode() == KEY_CODE_T) {
        rh::Renderer::ToggleToneMapping();
        return true;
    }
    if (e.GetKeyCode() == KEY_CODE_G) {
        rh::Renderer::ToggleGammaCorrection();
        return true;
    }
    if (e.GetKeyCode() == KEY_CODE_3) {
        m_3DScene.ToggleCollisionHulls();
        return true;
    }

    if (e.GetKeyCode() == KEY_CODE_M) {
        rh::Renderer::ToggleDebugSoundOutput();
        return true;
    }
    if (e.GetKeyCode() == KEY_CODE_J) {
        rh::Renderer::ToggleDebugControllerOutput();
        return true;
    }
    if (e.GetKeyCode() == KEY_CODE_BACKSLASH) {
        rh::Renderer::RecompileShaders();
        return true;
    }

    if (e.GetKeyCode() == KEY_CODE_P) {
        if (m_3DScene.IsPlaying()) {
            m_3DScene.OnRuntimePause();
        } else {
            m_3DScene.OnRuntimeResume();
        }
    }

    return false;
}

void Level::OnGuiRender() {
    // render debug ui
}

void Level::CheckViewportSize() {
    static rh::u32 specWidth = -1;
    static rh::u32 specHeight = -1;

    if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
        (specWidth != m_ViewportSize.x || specHeight != m_ViewportSize.y)) {
        specWidth = (rh::u32)m_ViewportSize.x;
        specHeight = (rh::u32)m_ViewportSize.y;
        ENGINE_LOG_INFO("Render resolution: {0} x {1}", specWidth, specHeight);
        rh::TextRenderer::OnWindowResize(specWidth, specHeight);
        rh::Renderer::OnWindowResize(specWidth, specHeight);
        m_3DScene.OnViewportResize(specWidth, specHeight);
    }
}