#ifndef INPUT_H
#define INPUT_H

#include <unordered_map>
#include <string>

#include <GLFW\glfw3.h>

#include "GameMath.hpp"

class Input {
public:

    static bool getKeyState(std::string key);
    static void watchKey(std::string key, int glfwKeyCode);
    static void pollKeys(GLFWwindow* window);

    static std::unordered_map<std::string, bool> m_keyStates;
    static std::unordered_map<std::string, int> m_watchedKeys;
    static math::vec2 m_mouseMove;

    static math::vec2 m_mouseAcc;

private:
    Input();
};

#endif