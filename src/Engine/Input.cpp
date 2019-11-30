#include "Input.hpp"
#include "GameObject\GameObject.hpp"

std::unordered_map<std::string, bool> Input::m_keyStates;
std::unordered_map<std::string, int> Input::m_watchedKeys;
math::vec2 Input::m_mouseMove;
math::vec2 Input::m_mouseAcc;
std::vector<GameObject*> Input::m_GameObjects;
bool Input::ShouldGameObjectHandleInputEvent = true;

bool Input::getKeyState(std::string key) {
    if (m_keyStates.find(key) == m_keyStates.end()) {
        return false;
    }
    else {
        return m_keyStates[key];
    }
}

void Input::watchKey(std::string key, int glfwKeyCode) {
    if (m_keyStates.find(key) == m_keyStates.end()) {
        // not already tracked

        m_keyStates[key] = false;
        m_watchedKeys[key] = glfwKeyCode;
    }
}

void Input::pollKeys(GLFWwindow* window) {
    for (auto k : m_watchedKeys) {
        m_keyStates[k.first] = 
            (glfwGetKey(window, k.second) == GLFW_PRESS);
    }
    m_mouseMove = m_mouseAcc;
    m_mouseAcc = math::vec2();
}

void Input::registerGameObject(GameObject* obj) {
    if (obj) {
        m_GameObjects.push_back(obj);
    }
}

void Input::handleMessage(Message msg) {
    if (ShouldGameObjectHandleInputEvent) {
        if (msg.isType("InputKey")) {
            using dt = Message::Datatype;

            dt key = msg.data[0];
            dt scancode = msg.data[1];
            dt action = msg.data[2];
            dt mods = msg.data[3];

            for (GameObject* go : m_GameObjects) {
                go->InputEvent(key, action);
            }
        }
    }
}

void Input::setHandleInput(bool val) {
    ShouldGameObjectHandleInputEvent = val;
}