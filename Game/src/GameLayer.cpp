#include "GameLayer.hpp"

#include "Engine/Core/Application.hpp"

/* native scripts */
#include "CameraController.hpp"
#include "Gem.hpp"

bool Engine::BindGameScript(const std::string& script_tag, Engine::Scene* scene, GameObject gameobject) {
    if (script_tag.compare("script_camera_controller") == 0) {
        gameobject.AddComponent<Engine::NativeScriptComponent>().Bind<CameraController>(gameobject);
            return true;
    }
    return false;
}

GameLayer::GameLayer() : EngineLayer("Game") {
}

void GameLayer::OnAttach() {
    LOG_INFO("Game layer attached");

    Input::CaptureMouse(true);
    
    m_ViewportSize = { 
        (float)Engine::Application::Get().GetWindow().GetWidth(), 
        (float)Engine::Application::Get().GetWindow().GetHeight() };

    /* 
    This section can be simplified to "load all the data needs to run"

    1. load all resources into catalogs/libraries
    2. load scene from file and deserialze

    */

    m_ActiveScene = std::make_shared<Engine::Scene>();
    //m_ActiveScene->loadFromFile("run_tree/Data/Levels/nbtTest.scene");

    Engine::MeshCatalog::Register("mesh_cube", "run_tree/Data/Models/cube.nbt", true);
    Engine::MeshCatalog::Register("mesh_ball", "run_tree/Data/Models/sphere.nbt", true);
    //Engine::MeshCatalog::Register("mesh_cyl", "run_tree/Data/Models/cylinder.nbt", true);
    //Engine::MeshCatalog::Register("mesh_helmet", "run_tree/Data/Models/helmet.nbt", true); // TODO: this is slow
    for (int nx = 0; nx < 5; nx++) {
        auto ball = m_ActiveScene->CreateGameObject("ball " + nx);
        auto mesh = Engine::MeshCatalog::Get("mesh_ball");
        ball.AddComponent<Engine::MeshRendererComponent>(mesh);

        auto& trans = ball.GetComponent<Engine::TransformComponent>().Transform;
        trans = mat4();
        trans.translate(vec3(nx-2, 1.0f, 0));
        trans.scale(vec3(.45f, .45f, .45f));
    }
    { // Platform
        auto platform = m_ActiveScene->CreateGameObject("Platform");
        auto rectMesh = Engine::MeshCatalog::Get("mesh_cube");
        platform.AddComponent<Engine::MeshRendererComponent>(rectMesh);

        auto& trans = platform.GetComponent<Engine::TransformComponent>().Transform;
        trans = mat4();
        trans.scale(vec3(5.0f, .25f, 5.0f));
        trans.translate(vec3(0, -1.0f, 0));
    }

    { // Lights
        auto light = m_ActiveScene->CreateGameObject("light 1");
        //light.AddComponent<Engine::LightComponent>(Engine::LightType::Point, vec3(1,1,1), 2, 0, 0);
        auto& trans = light.GetComponent<Engine::TransformComponent>().Transform;
        trans = mat4();
        trans.translate(vec3(0, 2, 0));
    }
    {
        auto light = m_ActiveScene->CreateGameObject("light 2");
        //light.AddComponent<Engine::LightComponent>(Engine::LightType::Point, vec3(.2, .8, .6), 4, 0, 0);
        auto& trans = light.GetComponent<Engine::TransformComponent>().Transform;
        trans = mat4();
        trans.translate(vec3(2.5, 3.6, 2));
    }
    {
        auto light = m_ActiveScene->CreateGameObject("Sun");
        // 255, 236, 224
        light.AddComponent<Engine::LightComponent>(Engine::LightType::Directional, vec3(1.0f, 236.0f/255.0f, 225.0f/255.0f), 5, 0, 0);
        auto& trans = light.GetComponent<Engine::TransformComponent>().Transform;
        //trans = mat4();
        trans = math::createYawPitchRollMatrix(45, 0, -80);
    }

    { // Player
        m_Camera = m_ActiveScene->CreateGameObject("Camera");
        m_Camera.AddComponent<Engine::CameraComponent>().camera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
        m_Camera.AddComponent<Engine::NativeScriptComponent>().Bind<CameraController>(m_Camera);
        //m_Camera.AddComponent<Engine::LightComponent>(Engine::LightType::Spot, vec3(0, 1, 0), 1, cos(d2r * 12.5), cos(d2r * 17.5));

        auto& trans = m_Camera.GetComponent<Engine::TransformComponent>().Transform;
        trans = mat4();
        trans.translate(vec3(0, 1, 2));
    }

    if (false) {
        // Load all meshes into the game
        {
            // this should be read from some sort of resource pack file
            std::vector<std::pair<std::string, std::string>> meshList = {
                { "cube_mesh", "run_tree/Data/Models/cube.mesh" },
            { "rect_mesh", "run_tree/Data/Models/cube.mesh" },
            { "guard_mesh", "run_tree/Data/Models/guard.nbt" }

            };
            for (auto p : meshList) {
                Engine::MeshCatalog::Register(p.first, p.second, false);
            }
        }

        { // Platform
            auto platform = m_ActiveScene->CreateGameObject("Platform");
            auto rectMesh = Engine::MeshCatalog::Get("cube_mesh");
            platform.AddComponent<Engine::MeshRendererComponent>(rectMesh);

            auto& trans = platform.GetComponent<Engine::TransformComponent>().Transform;
            trans = mat4();
            trans.scale(vec3(5.0f, .25f, 5.0f));
            trans.translate(vec3(0, -1.0f, 0));
        }
        { // Frog Cube 1
            auto frog = m_ActiveScene->CreateGameObject("Frog Cube 1");
            auto cubeMesh = Engine::MeshCatalog::Get("cube_mesh");
            frog.AddComponent<Engine::MeshRendererComponent>(cubeMesh);
            frog.AddComponent<Engine::NativeScriptComponent>().Bind<Gem>(frog);

            auto& trans = frog.GetComponent<Engine::TransformComponent>().Transform;
            trans = mat4();
            trans.translate(vec3(2, 1, -2));
        }
        { // Frog Cube 2
            auto frog = m_ActiveScene->CreateGameObject("Frog Cube 2");
            auto cubeMesh = Engine::MeshCatalog::Get("cube_mesh");
            frog.AddComponent<Engine::MeshRendererComponent>(cubeMesh);
            frog.AddComponent<Engine::NativeScriptComponent>().Bind<Gem>(frog);

            auto& trans = frog.GetComponent<Engine::TransformComponent>().Transform;
            trans = mat4();
            trans.translate(vec3(-2, 1, -2));
        }
        { // Frog Cube 3
            auto frog = m_ActiveScene->CreateGameObject("Frog Cube 3");
            //auto mesh = std::make_shared<Engine::Mesh>("run_tree/Data/Models/robot.nbt", true);
            auto mesh = std::make_shared<Engine::Mesh>("run_tree/Data/Models/blimp.nbt", true);
            frog.AddComponent<Engine::MeshRendererComponent>(mesh);
            frog.AddComponent<Engine::NativeScriptComponent>().Bind<Gem>(frog);

            auto& trans = frog.GetComponent<Engine::TransformComponent>().Transform;
            trans = mat4();
            trans.translate(vec3(2, 1, 2));
        }

        { // Player
            m_Camera = m_ActiveScene->CreateGameObject("Camera");
            m_Camera.AddComponent<Engine::CameraComponent>().camera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
            m_Camera.AddComponent<Engine::NativeScriptComponent>().Bind<CameraController>(m_Camera);

            auto& trans = m_Camera.GetComponent<Engine::TransformComponent>().Transform;
            trans = mat4();
            trans.translate(vec3(0, 1, 5));
        }
    }

    //m_ActiveScene->writeToFile("output.scene");

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
    }

    // toggle mouse controll for camera
    if (e.GetKeyCode() == KEY_CODE_TAB) {
        if (Input::IsMouseCaptured())
            Input::CaptureMouse(false);
        else
            Input::CaptureMouse(true);

        if (m_Camera) {
            auto camController = m_Camera.GetComponent<NativeScriptComponent>().GetScript<CameraController>();
            camController->ExternalAcess(); // test
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
    return false;
}

void GameLayer::OnGuiRender() {
}
