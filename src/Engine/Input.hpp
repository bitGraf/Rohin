#ifndef INPUT_H
#define INPUT_H

#include <unordered_map>
#include <string>

#include <GLFW/glfw3.h>
#include <vector>

#include "GameMath.hpp"
#include "Message/Message.hpp"

class GameObject;

class Input {
public:

    static void setupBindings();

    static bool getKeyState(std::string key);
    static f32 getAxisState(std::string key);
    static bool getLeftMouse();
    static bool getRightMouse();

    static void watchKey(std::string key, int glfwKeyCode);
    static void createAxis(std::string axisName, 
        int glfwKeyCodePlus, int glfwKeyCodeMinus,
        int glfwKeyCodePlus_alt, int glfwKeyCodeMinus_alt,
        int glfwGamepadPlus, int glfwGamepadMinus,
        int glfwGamepadPlus_alt, int glfwGamepadMinus_alt,
        int glfwGamepadAxis, bool flipAxis = false);
    static void pollKeys(GLFWwindow* window, double dt);
    static void registerGameObject(GameObject* obj);
    static void handleMessage(Message msg);
    static void setHandleInput(bool val);

    static math::vec2 m_mouseMove;
    static math::vec2 m_mouseAcc;
    static bool gamepadPresent;
    static float axisDeadzone;

private:
    struct Axis {
        // keyboard buttons
        int kPlus;
        int kMinus;

        int kPlus_2;
        int kMinus_2;

        // gamepad buttons
        int gpPlus;
        int gpMinus;

        int gpPlus_2;
        int gpMinus_2;

        // Analog axes
        int gpAxis;

        // options
        bool flipAxis;

        Axis() : 
            kPlus(0), kMinus(0), 
            kPlus_2(0), kMinus_2(0),
            gpPlus(-1), gpMinus(-1),
            gpPlus_2(-1), gpMinus_2(-1),
            gpAxis(-1), flipAxis(false) {}
        Axis(int a, int b, int c, int d, 
            int e, int f, int g, int h,
            int i, bool j) : 
            kPlus(a), kMinus(b), 
            kPlus_2(c), kMinus_2(d),
            gpPlus(e), gpMinus(f),
            gpPlus_2(g), gpMinus_2(h),
            gpAxis(i), flipAxis(j) {}
    };
    static std::unordered_map<std::string, bool> m_keyStates;
    static std::unordered_map<std::string, int> m_watchedKeys;

    static std::unordered_map<std::string, Axis> m_watchedAxisKeys;
    static std::unordered_map<std::string, f32> m_axes;

    static bool m_mouseLeft, m_mouseRight;

    static std::vector<GameObject*> m_GameObjects;

    static bool ShouldGameObjectHandleInputEvent;

    static unsigned char gamepadButtons[32];

    Input();
};

#endif
