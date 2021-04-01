#include "Level.hpp"

#include "MainMenu.hpp"

/* native scripts */
#include "../Scripts/CameraController.hpp"
#include "../Scripts/Player.hpp"
#include "../Scripts/Gem.hpp"

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

    {
        BENCHMARK_SCOPE("Loading Meshes");
        Engine::MeshCatalog::Register("mesh_guard", "Data/Models/guard.nbt", true);
        Engine::MeshCatalog::Register("mesh_plane", "Data/Models/plane.nbt", true);
    }
    { // Player
        auto player = m_3DScene->CreateGameObject("Player");
        auto mesh = Engine::MeshCatalog::Get("mesh_guard");
        player.AddComponent<Engine::MeshRendererComponent>(mesh);
        player.AddComponent<Engine::NativeScriptComponent>().Bind<PlayerController>(player);

        mesh->GetSubmeshes()[0].Transform = math::createYawPitchRollMatrix(90, 0, 0);

        auto& trans = player.GetComponent<Engine::TransformComponent>().Transform;
        trans = mat4();
        trans.translate(vec3(0, 1, 0));

        UID_t hull = cWorld.CreateNewCapsule(vec3(0, 1, 0) + vec3(0, .5, 0), 1, 0.5f);
        player.AddComponent<Engine::ColliderComponent>(hull);
    }
    { // Platform
        float platformSize = 20.0f;
        float platformThickness = 3.0f;

        auto platform = m_3DScene->CreateGameObject("Platform");

        auto rectMesh = Engine::MeshCatalog::Get("mesh_plane");
        auto material = rectMesh->GetMaterial(0);
        material->Set<float>("u_TextureScale", platformSize);
        material->Set("u_AlbedoTexture", Engine::Texture2D::Create("Data/Images/grid/PNG/Dark/texture_07.png"));
        platform.AddComponent<Engine::MeshRendererComponent>(rectMesh);

        auto& trans = platform.GetComponent<Engine::TransformComponent>().Transform;
        trans = mat4();
        trans.scale(vec3(platformSize, 1, platformSize));
        trans.translate(vec3(0, 0.0f, 0));

        UID_t floor = cWorld.CreateNewCubeHull(vec3(0, -platformThickness / 2.0f, 0), 2 * platformSize, platformThickness, 2 * platformSize);
    }

    { // Lights
        auto light = m_3DScene->CreateGameObject("Sun");
        light.AddComponent<Engine::LightComponent>(Engine::LightType::Directional, vec3(1.0f, 236.0f / 255.0f, 225.0f / 255.0f), 5, 0, 0);
        auto& trans = light.GetComponent<Engine::TransformComponent>().Transform;
        trans = math::createYawPitchRollMatrix(45, 0, -80);
    }

    { // Camera
        m_Camera = m_3DScene->CreateGameObject("Camera");
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
        Engine::SoundEngine::CreateSoundCue("guard_death", { "Data/Sounds/death.ogg", 0.02f });
        Engine::SoundEngine::CreateSoundCue("golem", { "Data/Sounds/golem.ogg", 0.1f }); //MONO, has 3D sound
        Engine::SoundEngine::CreateSoundCue("protector", { "Data/Sounds/sound.wav", 0.2f });
        Engine::SoundEngine::CreateSoundCue("ahhh", { "Data/Sounds/ahhh.ogg", 0.1f, 15.0f });
    }

    // Start the 3D scene
    m_3DScene->OnRuntimeStart();
}

void Level::OnDetach() {
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