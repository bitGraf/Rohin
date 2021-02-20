#pragma once

#include "Engine/Core/Base.hpp"

namespace Engine {

    class Input {
    public:
        /* public platform-independent interface */
        static bool IsKeyPressed(int keycode);

        static bool IsMouseButtonPressed(int button);
        static std::pair<float, float> GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();

        static bool IsMouseCaptured();
        static void CaptureMouse(bool capture = true);
    };
}