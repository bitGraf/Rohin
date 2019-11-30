#include "Input.hpp"
#include "GameObject\GameObject.hpp"

std::unordered_map<std::string, bool> Input::m_keyStates;
std::unordered_map<std::string, int> Input::m_watchedKeys;
math::vec2 Input::m_mouseMove;
math::vec2 Input::m_mouseAcc;
std::vector<GameObject*> Input::m_GameObjects;
bool Input::ShouldGameObjectHandleInputEvent = true;
std::unordered_map<std::string, Input::Axis> Input::m_watchedAxisKeys;
std::unordered_map<std::string, f32> Input::m_axes;

bool Input::getKeyState(std::string key) {
    if (m_keyStates.find(key) == m_keyStates.end()) {
        return false;
    }
    else {
        return m_keyStates[key];
    }
}

f32 Input::getAxisState(std::string key) {
    if (m_axes.find(key) == m_axes.end()) {
        return 0;
    }
    else {
        return m_axes[key];
    }
}

void Input::watchKey(std::string key, int glfwKeyCode) {
    if (m_keyStates.find(key) == m_keyStates.end()) {
        // not already tracked

        m_keyStates[key] = false;
        m_watchedKeys[key] = glfwKeyCode;
    }
}

void Input::watchKeyAxis(std::string axisName, int glfwKeyCodePlus, int glfwKeyCodeMinus) {
    if (m_axes.find(axisName) == m_axes.end()) {
        // not already tracked

        m_axes[axisName] = 0;
        m_watchedAxisKeys[axisName] = Axis( glfwKeyCodePlus , glfwKeyCodeMinus );
    }
}

void Input::pollKeys(GLFWwindow* window, double dt) {
    m_mouseMove = m_mouseAcc;
    m_mouseAcc = math::vec2();
    for (auto k : m_watchedKeys) {
        m_keyStates[k.first] = 
            (glfwGetKey(window, k.second) == GLFW_PRESS);
    }
    for (auto k : m_watchedAxisKeys) {
        auto key = k.first;
        auto axis = k.second;

        bool keyPlus = glfwGetKey(window, axis.Plus) == GLFW_PRESS;
        bool keyMinus = glfwGetKey(window, axis.Minus) == GLFW_PRESS;

        auto keyValue = &m_axes[key];
        
        double rate = 10.0 * dt;
        if (keyPlus) {
            *keyValue += rate;

            if (*keyValue > 1.0)
                *keyValue = 1.0;
        }
        if (keyMinus) {
            *keyValue -= rate;

            if (*keyValue < -1.0)
                *keyValue = -1.0;
        }
        if (!keyPlus && !keyMinus) {
            if (*keyValue > 0.1) {
                *keyValue -= rate;
            }
            else if (m_axes[key] < -.1) {
                *keyValue += rate;
            }
            else {
                *keyValue = 0;
            }
        }
    }
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