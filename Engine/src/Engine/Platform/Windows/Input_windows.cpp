#include <enpch.hpp>
#include "Engine/Core/Input.hpp"

#include <Engine/Core/Application.hpp>
#include <glfw/glfw3.h>

namespace Engine {
    static bool s_IsMouseCaptured = false;
    static std::unordered_map<std::string, InputAxisSpec> s_InputMap;

    bool Input::IsKeyPressed(int keycode) {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

        auto state = glfwGetKey(window, keycode);

        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    void Input::BindAxis(const std::string& axisName, InputAxisSpec spec) {
        s_InputMap[axisName] = spec;
    }

    float Input::GetAxis(const std::string& axisName) {
        if (s_InputMap.find(axisName) == s_InputMap.end()) {
            ENGINE_LOG_WARN("Could not find input axis {0}", axisName);
            return 0;
        }

        const auto& spec = s_InputMap.at(axisName);
        return ((float)IsKeyPressed(spec.keycodePlus)) - ((float)IsKeyPressed(spec.keycodeMinus));
    }

    bool Input::IsMouseButtonPressed(int button) {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

        auto state = glfwGetMouseButton(window, button);

        return state == GLFW_PRESS;
    }

    float Input::GetMouseX() {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return xpos;
    }

    float Input::GetMouseY() {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return ypos;
    }

    std::pair<float,float> Input::GetMousePosition() {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return { xpos, ypos };
    }

    bool Input::IsMouseCaptured() {
        return s_IsMouseCaptured;
    }

    void Input::CaptureMouse(bool capture) {
        if (capture != s_IsMouseCaptured) {
            s_IsMouseCaptured = capture;

            auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
            glfwSetInputMode(window, GLFW_CURSOR, capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
    }
}