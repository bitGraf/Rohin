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
        int glfwGamepadAxis, int glfwGamepadPlus, int glfwGamepadMinus,
        bool flipAxis = false);
    static void pollKeys(GLFWwindow* window, double dt);
    static void registerGameObject(GameObject* obj);
    static void handleMessage(Message msg);
    static void setHandleInput(bool val);

    static math::vec2 m_mouseMove;
    static math::vec2 m_mouseAcc;
    static bool gamepadPresent;

private:
    struct Axis {
        // keyboard buttons
        int kPlus;
        int kMinus;

        // gamepad axes
        int gpPlus;
        int gpMinus;
        int gpAxis;

        // options
        bool flipAxis;

        Axis() : 
            kPlus(0), kMinus(0), 
            gpAxis(-1), gpPlus(0), gpMinus(0),
            flipAxis(false) {}
        Axis(int a, int b, int c, int d, int e, bool f) : 
            kPlus(a), kMinus(b), 
            gpAxis(c), gpPlus(d), gpMinus(e),
            flipAxis(f) {}
    };
    static std::unordered_map<std::string, bool> m_keyStates;
    static std::unordered_map<std::string, int> m_watchedKeys;

    static std::unordered_map<std::string, Axis> m_watchedAxisKeys;
    static std::unordered_map<std::string, f32> m_axes;

    static bool m_mouseLeft, m_mouseRight;

    static std::vector<GameObject*> m_GameObjects;

    static bool ShouldGameObjectHandleInputEvent;

    Input();
};

#endif
