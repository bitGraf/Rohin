#include "GameLayer.hpp"

#include "Engine/Core/Application.hpp"

/* native scripts */
#include "CameraController.hpp"
#include "Gem.hpp"

GameLayer::GameLayer() : EngineLayer("Game", true) {
}

void GameLayer::OnAttach() {
    LOG_INFO("Game layer attached");

    Engine::FramebufferSpecification spec;
    spec.Width = 1280.0f;
    spec.Height = 720.0f;
    m_Framebuffer = Engine::Framebuffer::Create(spec);

    Input::CaptureMouse(true);
    
    m_ViewportSize = { 
        (float)Engine::Application::Get().GetWindow().GetWidth(), 
        (float)Engine::Application::Get().GetWindow().GetHeight() };

    /* 
    This section can be simplified to "load all the data needs to run"

    1. load all resources into catalogs/libraries
    2. load scene from file and deserialze

    */

    // Load all meshes into the game
    {
        // this should be read from some sort of resource pack file
        std::vector<std::pair<std::string, std::string>> meshList = {
            { "cube_mesh", "run_tree/Data/Models/cube.mesh"},
            { "rect_mesh", "run_tree/Data/Models/cube.mesh" }
        };
        for (auto p : meshList) {
            Engine::MeshCatalog::Register(p.first, p.second);
        }
    }

    m_ActiveScene = std::make_shared<Engine::Scene>();

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
        auto cubeMesh = Engine::MeshCatalog::Get("cube_mesh");
        frog.AddComponent<Engine::MeshRendererComponent>(cubeMesh);
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

    //m_ActiveScene->writeToFile("output.scene");

    m_ActiveScene->OnRuntimeStart();
}

void GameLayer::OnDetach() {
    LOG_INFO("Game layer detached");
    m_ActiveScene->OnRuntimeStop();
}

void GameLayer::CheckViewportSize() {
    Engine::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
    if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
        (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y)) {
        m_Framebuffer->Resize((u32)m_ViewportSize.x, (u32)m_ViewportSize.y);
        m_ActiveScene->OnViewportResize((u32)m_ViewportSize.x, (u32)m_ViewportSize.y);
        Engine::Renderer::OnWindowResize((u32)m_ViewportSize.x, (u32)m_ViewportSize.y);
    }
}

void GameLayer::OnUpdate(Engine::Timestep ts) {
    CheckViewportSize();

    // Render
    if (!IsSwap()) m_Framebuffer->Bind();
    Engine::RenderCommand::SetClearColor(math::vec4(.1, .1, .1, 1));
    Engine::RenderCommand::Clear();

    m_ActiveScene->OnUpdate(ts);
    if (!IsSwap()) m_Framebuffer->Unbind();
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

        auto camController = m_Camera.GetComponent<NativeScriptComponent>().GetScript<CameraController>();
        camController->ExternalAcess(); // test
    }

    // Debug options
    if (e.GetKeyCode() == KEY_CODE_L) {
        m_ActiveScene->ToggleShowEntityLocations();
    }
    if (e.GetKeyCode() == KEY_CODE_N) {
        m_ActiveScene->ToggleShowNormals();
    }
    return false;
}

void GameLayer::OnGuiRender() {
}
