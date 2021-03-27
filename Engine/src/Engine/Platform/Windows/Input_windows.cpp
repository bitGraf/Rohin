#include <enpch.hpp>
#include "Engine/Core/Input.hpp"

#include <Engine/Core/Application.hpp>
#include <glfw/glfw3.h>

namespace Engine {
    static bool s_IsMouseCaptured = false;
    static std::unordered_map<std::string, InputAxisSpec> s_InputMap;
    GamepadState s_GamepadState;

    bool Input::IsKeyPressed(int keycode) {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

        auto state = glfwGetKey(window, keycode);

        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    const GamepadState& Input::GetState() {
        return s_GamepadState;
    }

    void Input::Poll(double dt) {
        if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
            if (!s_GamepadState.present) {
                s_GamepadState.name = glfwGetGamepadName(GLFW_JOYSTICK_1);
                ENGINE_LOG_INFO("Gamepad [{0}] connected", s_GamepadState.name);
                s_GamepadState.present = true;
            }

            // TODO: don't know how safe this might be, should be good tho ;)
            s_GamepadState.valid = glfwGetGamepadState(GLFW_JOYSTICK_1, (GLFWgamepadstate*)(&s_GamepadState));
        }
        else if (s_GamepadState.present) {
            ENGINE_LOG_INFO("Gamepad [{0}] disconnected", s_GamepadState.name);
            s_GamepadState.name = nullptr;
            s_GamepadState.present = false;
            s_GamepadState.valid = false;
        }
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
        float value = 0;
        if (spec.joystickAxis >= 0 && s_GamepadState.valid) {
            value = s_GamepadState.axes[spec.joystickAxis];
        }
        if (spec.keycodePlus >= 0 && spec.keycodeMinus >= 0) {
            auto plus = IsKeyPressed(spec.keycodePlus);
            auto minus = IsKeyPressed(spec.keycodeMinus);
            if (plus) {
                if (minus) value = 0;
                else value = 1;
            }
            else {
                if (minus) value = -1;
            }
        }
        return value;
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