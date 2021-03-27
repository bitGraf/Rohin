#pragma once

#include "Engine/Core/Base.hpp"

namespace Engine {

    struct InputAxisSpec {
        int keycodePlus;
        int keycodeMinus;
    };

    class Input {
    public:
        /* public platform-independent interface */
        static bool IsKeyPressed(int keycode);

        static void BindAxis(const std::string& axisName, InputAxisSpec spec);
        static float GetAxis(const std::string& axisName);

        static bool IsMouseButtonPressed(int button);
        static std::pair<float, float> GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();

        static bool IsMouseCaptured();
        static void CaptureMouse(bool capture = true);
    };
}