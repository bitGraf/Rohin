#include "MenuLayer.hpp"

#include "Engine/Core/Application.hpp"


MenuLayer::MenuLayer() : EngineLayer("Menu") {
}

void MenuLayer::createMenu() {
    m_Buttons.emplace_back(490, 150, 300.0f, 75.0f, "Button1");
    m_Buttons.emplace_back(490, 250, 300.0f, 75.0f, "Button2");
    m_Buttons.emplace_back(540, 350, 200.0f, 75.0f, "Button3");
}

void MenuLayer::OnAttach() {
    LOG_INFO("Menu layer attached");
}

void MenuLayer::OnDetach() {
    LOG_INFO("Game layer detached");
}

void MenuLayer::OnUpdate(Engine::Timestep ts) {
    Engine::RenderCommand::SetClearColor(math::vec4(.1, .1, .1, 1));
    Engine::RenderCommand::Clear();
}

void MenuLayer::OnEvent(Engine::Event& event) {
    Engine::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Engine::KeyPressedEvent>(BIND_EVENT_FN(MenuLayer::OnKeyPressedEvent));
    dispatcher.Dispatch<Engine::MouseButtonReleasedEvent>(BIND_EVENT_FN(MenuLayer::OnMouseButtonReleasedEvent));
    dispatcher.Dispatch<Engine::MouseButtonPressedEvent>(BIND_EVENT_FN(MenuLayer::OnMouseButtonPressedEvent));
}

bool MenuLayer::OnMouseButtonPressedEvent(Engine::MouseButtonPressedEvent& e) {
    return false;
}

bool inBox(float x, float y, const Button& button) {
    float xmin = button.m_xmin;
    float xmax = xmin + button.m_width;

    float ymin = button.m_ymin;
    float ymax = ymin + button.m_height;

    if (xmin < x && x < xmax && ymin < y && y < ymax)
        return true;

    return false;
}

bool MenuLayer::OnMouseButtonReleasedEvent(Engine::MouseButtonReleasedEvent& e) {
    auto[x, y] = Engine::Input::GetMousePosition();

    for (const auto& but : m_Buttons) {
        if (inBox(x, y, but)) {
            ENGINE_LOG_INFO("{0} pressed!", but.m_Name);
        }
    }

    return false;
}

bool MenuLayer::OnKeyPressedEvent(Engine::KeyPressedEvent& e) {
    // respond to key press
    if (e.GetKeyCode() == KEY_CODE_ESCAPE) {
        // do something
        //Engine::Application::Get().Close();
    }

    return false;
}

void MenuLayer::OnGuiRender() {
    const auto& context = Engine::Application::Get().GetWindow().GetGraphicsContext();

    ImGui::Begin("Renderer");
    ImGui::Text("fps: %.0f, %.3f ms", 60.0f, 16.667f);
    if (ImGui::CollapsingHeader("Device Info")) {
        ImGui::Text("Vendor:   %s", context->GetVendorString());
        ImGui::Text("Device:   %s", context->GetDeviceString());
        ImGui::Text("Version:  %s", context->GetVersionString());
    }
    ImGui::End();

    float screenWidth = static_cast<float>(Engine::Application::Get().GetWindow().GetWidth());
    float screenHeight = static_cast<float>(Engine::Application::Get().GetWindow().GetHeight());
    float screenPosx = static_cast<float>(Engine::Application::Get().GetWindow().GetXpos());
    float screenPosy = static_cast<float>(Engine::Application::Get().GetWindow().GetYpos());

    for (const auto& but : m_Buttons) {
        auto k = ImGui::GetWindowPos();
        ImGui::SetNextWindowSize(ImVec2(but.m_width, but.m_height), ImGuiCond_Once);
        ImVec2 winPos(screenPosx + but.m_xmin, screenPosy + but.m_ymin);
        ImGui::SetNextWindowPos(winPos);
        ImGui::Begin(but.m_Name.c_str(), 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMouseInputs);
        ImGui::Text(but.m_Name.c_str());
        ImGui::End();
    }
}
