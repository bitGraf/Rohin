#include "enpch.hpp"
#include "Application.hpp"

#include "Engine/Core/Logger.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/TextRenderer.hpp"

#include <GLFW/glfw3.h>

namespace Engine {

    Application* Application::s_Instance = nullptr;

    Application::Application()  {
        ENGINE_LOG_ASSERT(!s_Instance, "App already exists");
        s_Instance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::HandleEvent));

        Renderer::Init();
        TextRenderer::Init();

        m_GuiLayer = new GuiLayer();
        PushOverlay(m_GuiLayer);
    }

    Application::~Application() {
        Renderer::Shutdown();
        TextRenderer::Shutdown();
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
        while (!m_Done) {
            float time = glfwGetTime(); // TODO: Get to platform
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

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