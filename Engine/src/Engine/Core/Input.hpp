#pragma once

#include "Engine/Core/Base.hpp"

namespace Engine {

    struct InputAxisSpec {
        int keycodePlus = -1;
        int keycodeMinus = -1;

        int joystickAxis = -1;
        bool negate = false;
    };

    struct GamepadState {
        /*! The states of each [gamepad button](@ref gamepad_buttons), `GLFW_PRESS`
         *  or `GLFW_RELEASE`.
         */
        u8 buttons[15];
        /*! The states of each [gamepad axis](@ref gamepad_axes), in the range -1.0
         *  to 1.0 inclusive.
         */
        f32 axes[6];

        bool present = false;
        bool valid = false;
        const char* name = nullptr;
        bool apply_dead_zone = true;
        float dead_zone = 0.1f;
    };

    class Input {
    public:
        /* public platform-independent interface */
        static bool IsKeyPressed(int keycode);

        static void Poll(double dt);
        static void BindAxis(const std::string& axisName, InputAxisSpec spec);
        static float GetAxis(const std::string& axisName);

        static const GamepadState& GetState();

        static bool IsMouseButtonPressed(int button);
        static std::pair<float, float> GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();

        static bool IsMouseCaptured();
        static void CaptureMouse(bool capture = true);
    };
}