#ifndef INPUT_H
#define INPUT_H

#include <unordered_map>
#include <string>

#include <GLFW\glfw3.h>

#include "GameMath.hpp"
#include "Message\Message.hpp"

class PlayerObject;

class Input {
public:

    static bool getKeyState(std::string key);
    static void watchKey(std::string key, int glfwKeyCode);
    static void pollKeys(GLFWwindow* window);    
    static void registerInputEventCallback(PlayerObject* obj);
    static void handleMessage(Message msg);

    static math::vec2 m_mouseMove;
    static math::vec2 m_mouseAcc;

private:
    static std::unordered_map<std::string, bool> m_keyStates;
    static std::unordered_map<std::string, int> m_watchedKeys;

    static std::vector<PlayerObject*> m_PlayerObjects;
    Input();
};

#endif