#include <enpch.hpp>
#include "Engine/Core/Application.hpp"

#include "Engine/Core/Logger.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/TextRenderer.hpp"
#include "Engine/Resources/MaterialCatalog.hpp"
#include "Engine/Sound/SoundEngine.hpp"

#include <GLFW/glfw3.h>

namespace Engine {

    Application* Application::s_Instance = nullptr;

    Application::Application()  {
        BENCHMARK_START_SESSION("Application Startup", "benchmark/startup.json");
        ENGINE_LOG_ASSERT(!s_Instance, "App already exists");
        s_Instance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::HandleEvent));

        Renderer::Init();
        TextRenderer::Init();
        MaterialCatalog::Init();
        SoundEngine::Init();

        m_GuiLayer = new GuiLayer();
        PushOverlay(m_GuiLayer);
        BENCHMARK_END_SESSION();
    }

    Application::~Application() {
        BENCHMARK_START_SESSION("Application Shutdown", "benchmark/shutdown.json");
        Renderer::Shutdown();
        TextRenderer::Shutdown();
        MaterialCatalog::Shutdown();
        SoundEngine::Shutdown();
        BENCHMARK_END_SESSION();
    }

    void Application::Close() {
        m_Done = true;
    }

    void Application::HandleEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

        for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
            (*--it)->OnEvent(event);
            if (event.Handled)
                break;
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

            if (!m_Minimized) {
                /* Run all engine layer updates */
                for (EngineLayer* layer : m_layerStack)
                    layer->OnUpdate(timestep);
            }

            /* put on render thread */
            m_GuiLayer->Begin();
            for (EngineLayer* layer : m_layerStack)
                layer->OnGuiRender();
            m_GuiLayer->End();

            m_Window->Update();
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

    void Application::PushLayer(EngineLayer* layer) {
        m_layerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(EngineLayer* overlay) {
        m_layerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }
}