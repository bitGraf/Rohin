#include "PauseMenuLayer.hpp"

#include "Engine/Core/Application.hpp"

#include "GameLayer.hpp"

PauseLayer::PauseLayer() : EngineLayer("PauseMenu") {
}

void PauseLayer::OnAttach() {
    LOG_INFO("Pause layer attached");

    m_Buttons.emplace_back(490, 150, 300.0f, 75.0f, "Resume");
    m_Buttons.emplace_back(490, 250, 300.0f, 75.0f, "Quit");
}

void PauseLayer::OnDetach() {
    LOG_INFO("Pause layer detached");
}

void PauseLayer::OnUpdate(Engine::Timestep ts) {
    Engine::RenderCommand::SetClearColor(math::vec4(.1, .1, .1, 1));
    Engine::RenderCommand::Clear();


    float screenWidth = static_cast<float>(Engine::Application::Get().GetWindow().GetWidth());
    float screenHeight = static_cast<float>(Engine::Application::Get().GetWindow().GetHeight());
    float screenPosx = static_cast<float>(Engine::Application::Get().GetWindow().GetXpos());
    float screenPosy = static_cast<float>(Engine::Application::Get().GetWindow().GetYpos());

    Engine::TextRenderer::SubmitText("Main Menu", screenWidth / 2.0f, 100, math::vec3(.2, .6, .9f), ALIGN_MID_MID);
    for (const auto& but : m_Buttons) {
        float xcenter = but.m_xmin + but.m_width / 2.0f;
        float ycenter = but.m_ymin + but.m_height / 2.0f;

        Engine::TextRenderer::SubmitText(but.m_Name, xcenter, ycenter, math::vec3(1, 1, 1), ALIGN_MID_MID);
    }
}

void PauseLayer::OnEvent(Engine::Event& event) {
    Engine::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Engine::KeyPressedEvent>(BIND_EVENT_FN(PauseLayer::OnKeyPressedEvent));
    dispatcher.Dispatch<Engine::MouseButtonReleasedEvent>(BIND_EVENT_FN(PauseLayer::OnMouseButtonReleasedEvent));
    dispatcher.Dispatch<Engine::MouseButtonPressedEvent>(BIND_EVENT_FN(PauseLayer::OnMouseButtonPressedEvent));
}

bool PauseLayer::OnMouseButtonPressedEvent(Engine::MouseButtonPressedEvent& e) {
    return false;
}

bool PauseLayer::inBox(float x, float y, const Button& button) {
    float xmin = button.m_xmin;
    float xmax = xmin + button.m_width;

    float ymin = button.m_ymin;
    float ymax = ymin + button.m_height;

    if (xmin < x && x < xmax && ymin < y && y < ymax)
        return true;

    return false;
}

bool PauseLayer::OnMouseButtonReleasedEvent(Engine::MouseButtonReleasedEvent& e) {
    auto[x, y] = Engine::Input::GetMousePosition();

    for (const auto& but : m_Buttons) {
        if (inBox(x, y, but)) {
            ENGINE_LOG_INFO("{0} pressed!", but.m_Name);

            if (but.m_Name.compare("Resume") == 0) {
                Engine::Application::Get().RemoveLayerNextFrame(this);
                m_LayerActive = false; // deactivate this layer for now
            } else if (but.m_Name.compare("Quit") == 0) {
                Engine::Application::Get().Close();
            }
        }
    }

    return false;
}

bool PauseLayer::OnKeyPressedEvent(Engine::KeyPressedEvent& e) {
    // respond to key press
    if (e.GetKeyCode() == KEY_CODE_ESCAPE) {
        // do something
        Engine::Application::Get().Close();
    }

    return false;
}

void PauseLayer::OnGuiRender() {
    const auto& context = Engine::Application::Get().GetWindow().GetGraphicsContext();

    ImGui::Begin("Renderer");
    ImGui::Text("fps: %.0f, %.3f ms", 60.0f, 16.667f);
    if (ImGui::CollapsingHeader("Device Info")) {
        ImGui::Text("Vendor:   %s", context->GetVendorString());
        ImGui::Text("Device:   %s", context->GetDeviceString());
        ImGui::Text("Version:  %s", context->GetVersionString());
    }
    ImGui::End();
}
