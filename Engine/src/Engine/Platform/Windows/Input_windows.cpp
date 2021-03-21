#include <enpch.hpp>
#include "Engine/Core/Input.hpp"

#include <Engine/Core/Application.hpp>
#include <glfw/glfw3.h>

namespace Engine {
    static bool s_IsMouseCaptured = false;

    bool Input::IsKeyPressed(int keycode) {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

        auto state = glfwGetKey(window, keycode);

        return state == GLFW_PRESS || state == GLFW_REPEAT;
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