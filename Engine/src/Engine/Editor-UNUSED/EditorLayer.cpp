#include <enpch.hpp>
/*
#include "EditorLayer.hpp"

#include "Engine/Core/Application.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/GameObject/Components.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Editor/EditorCameraController.hpp"

#include "imgui.h"

namespace Engine {

    EditorLayer::EditorLayer() : EngineLayer("Editor") {
    }

    void EditorLayer::OnAttach() {
        ENGINE_LOG_INFO("Editor layer attached");

        FramebufferSpecification spec;
        spec.Width = 1280.0f;
        spec.Height = 720.0f;
        m_Framebuffer = Framebuffer::Create(spec);

        //m_EditorScene = std::make_shared<Scene3D>();

        //auto frog = m_EditorScene->CreateGameObject("Frog Cube 2");
        //auto& mesh = frog.AddComponent<MeshRendererComponent>(std::make_shared<Engine::Mesh>("Data/Models/cube.mesh"));
        //mesh.Mesh->GetMaterial(0)->Set("u_color", math::vec4(1, .2, .1, 1));

        //m_EditorCamera = m_EditorScene->CreateGameObject("EditorCamera");
        //auto& camComp = m_EditorCamera.AddComponent<CameraComponent>();
        //camComp.Primary = true;

        //m_EditorCamera.AddComponent<NativeScriptComponent>().Bind<EditorCameraController>(m_EditorCamera);
        ///m_EditorCamera.AddNativeScript<EditorCameraController>();
        ///m_EditorCamera.AddNativeScript();

        if (m_GameLayer) {
            m_GameLayer->SetSwap(false);

            auto scene = m_GameLayer->GetScene();
            if (scene) {
                m_Panel.SetContext(scene);
            }
        }
    }
    void EditorLayer::OnDetach() {
        ENGINE_LOG_INFO("Editor layer detached");
    }

    void EditorLayer::CheckViewportSize() {
        FramebufferSpecification spec = m_Framebuffer->GetSpecification();
        if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y)) {
            m_Framebuffer->Resize((u32)m_ViewportSize.x, (u32)m_ViewportSize.y);
            //m_EditorScene->OnViewportResize((u32)m_ViewportSize.x, (u32)m_ViewportSize.y);
            Renderer::OnWindowResize((u32)m_ViewportSize.x, (u32)m_ViewportSize.y);
        }
    }

    void EditorLayer::OnUpdate(Timestep ts) {
        CheckViewportSize();

        // Update
        if (m_ViewportFocused && m_ViewportHovered) {
            // only update the scene if the viewport is active?
            //m_CameraController.OnUpdate(ts);
        }

        // Render
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor(math::vec4(.1, .1, .1, 1));
        RenderCommand::Clear();

        //m_EditorScene->OnUpdate(ts);

        m_Framebuffer->Unbind();
    }

    void EditorLayer::OnEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressedEvent));
    }

    bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& e) {
#if 0
        if (e.GetKeyCode() == KEY_CODE_F5) {
            m_EditorEnabled = !m_EditorEnabled;
            m_Framebuffer->GetSpecification();

            if (!m_EditorEnabled) {
                auto width = Application::Get().GetWindow().GetWidth();
                auto height = Application::Get().GetWindow().GetHeight();

                m_ViewportSize = { (float)width, (float)height };
                CheckViewportSize();
            }
        }
#endif

        if (e.GetKeyCode() == KEY_CODE_1) {
            m_GameLayer->GetScene()->OnRuntimeStart();
        }

        if (e.GetKeyCode() == KEY_CODE_2) {
            m_GameLayer->GetScene()->OnRuntimeStop();
        }

        return false;
    }

    void EditorLayer::OnGuiRender() {
        const auto& context = Application::Get().GetWindow().GetGraphicsContext();

        static bool dockspaceOpen = true;
        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen) {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Dockspace Demo", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        //Dockspace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit")) Application::Get().Close();

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // Render viewport window
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

        ImVec2 viewPortPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = { viewPortPanelSize.x, viewPortPanelSize.y };

        auto gameFB = m_GameLayer->GetOutput();
        u32 texID = m_Framebuffer->GetColorAttachmentID();
        if (gameFB)
            texID = gameFB->GetColorAttachmentID();
        ImGui::Image((void*)texID, viewPortPanelSize, ImVec2{ 0,1 }, ImVec2{ 1,0 });
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::Begin("Renderer");
        ImGui::Text("fps: %.0f, %.3f ms", 60.0f, 16.667f);
        if (ImGui::CollapsingHeader("Device Info")) {
            ImGui::Text("Vendor:   %s", context->GetVendorString());
            ImGui::Text("Device:   %s", context->GetDeviceString());
            ImGui::Text("Version:  %s", context->GetVersionString());
        }
        ImGui::End();

        // Render sceneHirearchyPanel
        if (!m_GameLayer->GetScene()->IsPlaying())
            m_Panel.OnImGuiRender();

        ImGui::End();
    }
}
*/
