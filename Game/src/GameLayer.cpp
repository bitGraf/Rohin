#include "GameLayer.hpp"

#include "Engine/Core/Application.hpp"

#include "Engine/Collision/CollisionWorld.hpp"

#include "Engine/Sound/SoundEngine.hpp"

#include "PauseMenuLayer.hpp"

/* native scripts */
#include "CameraController.hpp"
#include "Player.hpp"
#include "Gem.hpp"

bool Engine::BindGameScript(const std::string& script_tag, Engine::Scene3D* scene, GameObject gameobject) {
    if (script_tag.compare("script_camera_controller") == 0) {
        gameobject.AddComponent<Engine::NativeScriptComponent>().Bind<CameraController>(gameobject);
            return true;
    }
    return false;
}

GameLayer::GameLayer() : EngineLayer("Game") {
}

void GameLayer::OnAttach() {
    BENCHMARK_FUNCTION();
    LOG_INFO("Game layer attached");

    Input::CaptureMouse(false);
    Input::BindAxis("AxisMoveForward", { KEY_CODE_W, KEY_CODE_S, GAMEPAD_AXIS_LEFT_Y, true });
    Input::BindAxis("AxisMoveRight",   { KEY_CODE_D, KEY_CODE_A, GAMEPAD_AXIS_LEFT_X });
    Input::BindAxis("AxisRotateRight", { KEY_CODE_E, KEY_CODE_Q, GAMEPAD_AXIS_RIGHT_X });
    Input::BindAxis("AxisRotateUp",    { KEY_CODE_R, KEY_CODE_F, GAMEPAD_AXIS_RIGHT_Y, true });
    Input::BindAxis("AxisBoost",       { KEY_CODE_LEFT_SHIFT, KEY_CODE_F12, GAMEPAD_AXIS_LEFT_TRIGGER });

    Input::BindAction("ActionJump", {KEY_CODE_SPACE, GAMEPAD_BUTTON_A});
    
    m_ViewportSize = { 
        (float)Engine::Application::Get().GetWindow().GetWidth(), 
        (float)Engine::Application::Get().GetWindow().GetHeight() };

    /* 
    This section can be simplified to "load all the data needs to run"

    1. load all resources into catalogs/libraries
    2. load scene from file and deserialze

    */

    m_ActiveScene = std::make_shared<Engine::Scene3D>();
    {
        BENCHMARK_SCOPE("Loading Meshes");
        Engine::MeshCatalog::Register("mesh_guard", "Data/Models/guard.nbt", true);
        Engine::MeshCatalog::Register("mesh_plane", "Data/Models/plane.nbt", true);
    }
    { // Player
        auto player = m_ActiveScene->CreateGameObject("Player");
        auto mesh = MeshCatalog::Get("mesh_guard");
        player.AddComponent<Engine::MeshRendererComponent>(mesh);
        player.AddComponent<Engine::NativeScriptComponent>().Bind<PlayerController>(player);

        mesh->GetSubmeshes()[0].Transform = math::createYawPitchRollMatrix(90, 0, 0);

        auto& trans = player.GetComponent<Engine::TransformComponent>().Transform;
        trans = mat4();
        trans.translate(vec3(0, 1, 0));

        UID_t hull = cWorld.CreateNewCapsule(vec3(0, 1, 0) + vec3(0,.5,0), 1, 0.5f);
        player.AddComponent<Engine::ColliderComponent>(hull);
    }
    { // Platform
        float platformSize = 20.0f;
        float platformThickness = 3.0f;

        auto platform = m_ActiveScene->CreateGameObject("Platform");

        auto rectMesh = Engine::MeshCatalog::Get("mesh_plane");
        auto material = rectMesh->GetMaterial(0);
        material->Set<float>("u_TextureScale", platformSize);
        material->Set("u_AlbedoTexture", Engine::Texture2D::Create("Data/Images/grid/PNG/Dark/texture_07.png"));
        platform.AddComponent<Engine::MeshRendererComponent>(rectMesh);

        auto& trans = platform.GetComponent<Engine::TransformComponent>().Transform;
        trans = mat4();
        trans.scale(vec3(platformSize, 1, platformSize));
        trans.translate(vec3(0, 0.0f, 0));

        UID_t floor = cWorld.CreateNewCubeHull(vec3(0, -platformThickness/2.0f, 0), 2*platformSize, platformThickness, 2*platformSize);
    }

    { // Lights
        auto light = m_ActiveScene->CreateGameObject("Sun");
        light.AddComponent<Engine::LightComponent>(Engine::LightType::Directional, vec3(1.0f, 236.0f/255.0f, 225.0f/255.0f), 5, 0, 0);
        auto& trans = light.GetComponent<Engine::TransformComponent>().Transform;
        trans = math::createYawPitchRollMatrix(45, 0, -80);
    }

    { // Camera
        m_Camera = m_ActiveScene->CreateGameObject("Camera");
        auto& camera = m_Camera.AddComponent<Engine::CameraComponent>().camera;
        camera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
        camera.SetPerspective(75, .01, 100);
        m_Camera.AddComponent<Engine::NativeScriptComponent>().Bind<CameraController>(m_Camera);

        auto& trans = m_Camera.GetComponent<Engine::TransformComponent>().Transform;
        trans = mat4();
        trans.translate(vec3(0, 4, 5));
        trans *= math::createYawPitchRollMatrix(0, 0, -45);
    }

    // Ramps at various angles
    cWorld.getHullFromID(cWorld.CreateNewCubeHull(vec3(5, 0, -5), 10, 1, 3))
        ->rotation.toYawPitchRoll(0, 10, 0);
    cWorld.getHullFromID(cWorld.CreateNewCubeHull(vec3(5, 1, -2), 10, 1, 3))
        ->rotation.toYawPitchRoll(0, 20, 0);
    cWorld.getHullFromID(cWorld.CreateNewCubeHull(vec3(5, 2, 1), 10, 1, 3))
        ->rotation.toYawPitchRoll(0, 30, 0);
    cWorld.getHullFromID(cWorld.CreateNewCubeHull(vec3(5, 3, 4), 10, 1, 3))
        ->rotation.toYawPitchRoll(0, 40, 0);
    cWorld.getHullFromID(cWorld.CreateNewCubeHull(vec3(5, 3.5, 7), 10, 1, 3))
        ->rotation.toYawPitchRoll(0, 50, 0);

    // Sound stuff
    {
        BENCHMARK_SCOPE("Create soundcues");
        SoundEngine::CreateSoundCue("guard_death", { "Data/Sounds/death.ogg", 0.02f });
        SoundEngine::CreateSoundCue("golem", { "Data/Sounds/golem.ogg", 0.1f }); //MONO, has 3D sound
        SoundEngine::CreateSoundCue("protector", { "Data/Sounds/sound.wav", 0.2f });
        SoundEngine::CreateSoundCue("ahhh", { "Data/Sounds/ahhh.ogg", 0.1f, 15.0f });
    }

    m_ActiveScene->OnRuntimeStart();
}

void GameLayer::OnDetach() {
    LOG_INFO("Game layer detached");
    m_ActiveScene->OnRuntimeStop();
}

void GameLayer::CheckViewportSize() {
    static u32 specWidth = -1;
    static u32 specHeight = -1;

    if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
        (specWidth != m_ViewportSize.x || specHeight != m_ViewportSize.y)) {
        specWidth = (u32)m_ViewportSize.x;
        specHeight = (u32)m_ViewportSize.y;
        ENGINE_LOG_INFO("Render resolution: {0} x {1}", specWidth, specHeight);
        m_ActiveScene->OnViewportResize((u32)m_ViewportSize.x, (u32)m_ViewportSize.y);
        Engine::Renderer::OnWindowResize((u32)m_ViewportSize.x, (u32)m_ViewportSize.y);
    }
}

void GameLayer::OnUpdate(Engine::Timestep ts) {
    CheckViewportSize();

    // Render
    Engine::RenderCommand::SetClearColor(math::vec4(.1, .1, .1, 1));
    Engine::RenderCommand::Clear();

    m_ActiveScene->OnUpdate(ts);
}

void GameLayer::OnEvent(Engine::Event& event) {
    Engine::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Engine::KeyPressedEvent>(BIND_EVENT_FN(GameLayer::OnKeyPressedEvent));

    dispatcher.Dispatch<Engine::WindowResizeEvent>([&](Engine::WindowResizeEvent& e) {
        m_ViewportSize = {
            (float)e.GetWidth(),
            (float)e.GetHeight()
        };

        return false;
    });
}

bool GameLayer::OnKeyPressedEvent(Engine::KeyPressedEvent& e) {
    // quit game
    if (e.GetKeyCode() == KEY_CODE_ESCAPE) {
        Engine::Application::Get().Close();

        //PauseLayer* pause = new PauseLayer();
        //Engine::Application::Get().PushLayerNextFrame(pause);
        //m_LayerActive = false;
    }

    // toggle mouse controll for camera
    if (e.GetKeyCode() == KEY_CODE_TAB) {
        if (Input::IsMouseCaptured())
            Input::CaptureMouse(false);
        else
            Input::CaptureMouse(true);
    }

    if (e.GetKeyCode() == KEY_CODE_C) {
        if (m_Camera) {
            if (m_Camera.HasComponent<NativeScriptComponent>()) {
                auto scriptComp = m_Camera.GetComponent<NativeScriptComponent>();
                auto script = scriptComp.GetScript<CameraController>();
                script->ToggleControl();
            }
        }

        auto player = m_ActiveScene->FindByName("Player");
        if (player) {
            auto script = player.GetComponent<NativeScriptComponent>().GetScript<PlayerController>();
            script->ToggleControl();
        }
    }

    // Debug options
    if (e.GetKeyCode() == KEY_CODE_L) {
        m_ActiveScene->ToggleShowEntityLocations();
    }
    if (e.GetKeyCode() == KEY_CODE_N) {
        m_ActiveScene->ToggleShowNormals();
    }
    if (e.GetKeyCode() == KEY_CODE_P) {
        bool playing = m_ActiveScene->IsPlaying();

        if (playing) m_ActiveScene->OnRuntimePause();
        else         m_ActiveScene->OnRuntimeResume();
    }

    if (e.GetKeyCode() == KEY_CODE_O) {
        Renderer::NextOutputMode();
    }
    if (e.GetKeyCode() == KEY_CODE_I) {
        Renderer::PrintState();
    }
    if (e.GetKeyCode() == KEY_CODE_T) {
        Renderer::ToggleToneMapping();
    }
    if (e.GetKeyCode() == KEY_CODE_G) {
        Renderer::ToggleGammaCorrection();
    }
    if (e.GetKeyCode() == KEY_CODE_3) {
        m_ActiveScene->ToggleCollisionHulls();
    }

    if (e.GetKeyCode() == KEY_CODE_KP_1) {
        Engine::SoundEngine::CueSound("guard_death");
    }
    if (e.GetKeyCode() == KEY_CODE_KP_2) {
        Engine::SoundEngine::CueSound("protector");
    }
    if (e.GetKeyCode() == KEY_CODE_KP_3) {
        Engine::SoundEngine::CueSound("ahhh");
    }
    if (e.GetKeyCode() == KEY_CODE_KP_4) {
        Engine::SoundEngine::CueSound("golem");
    }
    if (e.GetKeyCode() == KEY_CODE_KP_5) {
        auto player = m_ActiveScene->FindByName("Player");
        if (player) {
            auto trans = player.GetComponent<TransformComponent>().Transform;
            auto[_pos, _yaw, _pitch, _roll, _forward, _right, _up] = math::Decompose(trans);
            Engine::SoundEngine::CueSound("golem", _pos);
        }
    }
    if (e.GetKeyCode() == KEY_CODE_KP_6) {
        Engine::SoundEngine::StopSteam();
    }
    if (e.GetKeyCode() == KEY_CODE_M) {
        Renderer::ToggleDebugSoundOutput();
    }
    if (e.GetKeyCode() == KEY_CODE_J) {
        Renderer::ToggleDebugControllerOutput();
    }
    if (e.GetKeyCode() == KEY_CODE_BACKSLASH) {
        Renderer::RecompileShaders();
    }
    return false;
}

void GameLayer::OnGuiRender() {
}
