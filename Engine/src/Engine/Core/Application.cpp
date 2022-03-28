#include <enpch.hpp>
#include "Engine/Core/Application.hpp"

#include "Engine/Core/Logger.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/TextRenderer.hpp"
#include "Engine/Renderer/SpriteRenderer.hpp"
#include "Engine/Sound/SoundEngine.hpp"

#include "Engine/Resources/MaterialCatalog.hpp"
#include "Engine/Resources/MeshCatalog.hpp"
#include "Engine/Resources/ResourceManager.hpp"

#include <GLFW/glfw3.h>

namespace Engine {

    Application* Application::s_Instance = nullptr;

    Application::Application()  {
        BENCHMARK_START_SESSION("Application Startup", "benchmark/startup.json");
        ENGINE_LOG_ASSERT(!s_Instance, "App already exists");
        s_Instance = this;

        PrintConfiguration();

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::HandleEvent));

        ResourceManager::CreateBuffers();

        Renderer::Init();
        TextRenderer::Init();
        SpriteRenderer::Init();
        SoundEngine::Init();

        MaterialCatalog::Create();
        MeshCatalog::Create();

        // create ImGUI renderer
        m_GuiLayer = new GuiLayer();
        m_GuiLayer->OnAttach();
        BENCHMARK_END_SESSION();
    }

    Application::~Application() {
        BENCHMARK_START_SESSION("Application Shutdown", "benchmark/shutdown.json");
        if (m_CurrentScene) {
            m_CurrentScene->OnDetach();
            delete m_CurrentScene;
            m_CurrentScene = nullptr;
        }
        MeshCatalog::Destroy();
        MaterialCatalog::Destroy();
        SoundEngine::Shutdown();
        SpriteRenderer::Shutdown();
        TextRenderer::Shutdown();
        Renderer::Shutdown();

        ResourceManager::DestroyBuffers();

        m_GuiLayer->OnDetach();
        BENCHMARK_END_SESSION();
    }

    void Application::Close() {
        m_Done = true;
    }

    void Application::HandleEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

        if (m_CurrentScene) {
            m_CurrentScene->OnEvent(event);
            //if (event.Handled) {
            //    // do something I guess?
            //}
        }
    }

    void Application::Run() {
        BENCHMARK_START_SESSION("Runtime", "benchmark/runtime.json");
        while (!m_Done) {
            BENCHMARK_SCOPE("Update");
            float time = glfwGetTime(); // TODO: Get to platform
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            SoundEngine::Update(timestep);
            Input::Poll(timestep);

            if (!m_Minimized) {
                /* Run all engine layer updates */
                if (m_CurrentScene) {
                    m_CurrentScene->OnUpdate(timestep);

                    m_GuiLayer->Begin();
                    m_CurrentScene->OnGuiRender();
                    m_GuiLayer->End();
                }
            }

            m_Window->Update();

            // transition scene
            if (m_NextScene) {
                if (m_CurrentScene) {
                    // remove current scene
                    m_CurrentScene->OnDetach();
                    delete m_CurrentScene;
                    m_CurrentScene = nullptr;
                }

                m_CurrentScene = m_NextScene;;
                m_CurrentScene->OnAttach();
                m_NextScene = nullptr;
            }
        }
        BENCHMARK_END_SESSION();
    }

    bool Application::OnWindowClose(WindowCloseEvent& event) {
        m_Done = true;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& event) {
        if (event.GetWidth() == 0 || event.GetHeight() == 0) {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;
        Renderer::OnWindowResize(event.GetWidth(), event.GetHeight());

        return false;
    }

    void Application::PushNewScene(Scene* newScene) {
        m_NextScene = newScene;
    }
}