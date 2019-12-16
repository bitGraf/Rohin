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

    static bool getKeyState(std::string key);
    static f32 getAxisState(std::string key);
    static bool getLeftMouse();
    static bool getRightMouse();

    static void watchKey(std::string key, int glfwKeyCode);
    static void watchKeyAxis(std::string axisName, int glfwKeyCodePlus, int glfwKeyCodeMinus);
    static void pollKeys(GLFWwindow* window, double dt);
    static void registerGameObject(GameObject* obj);
    static void handleMessage(Message msg);
    static void setHandleInput(bool val);

    static math::vec2 m_mouseMove;
    static math::vec2 m_mouseAcc;

private:
    struct Axis {
        int Plus;
        int Minus;
        Axis() : Plus(0), Minus(0) {}
        Axis(int a, int b) : Plus(a), Minus(b) {}
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
