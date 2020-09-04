#include "Input.hpp"
#include "GameObject/GameObject.hpp"

std::unordered_map<std::string, bool> Input::m_keyStates;
std::unordered_map<std::string, int> Input::m_watchedKeys;
std::vector<GameObject*> Input::m_GameObjects;
bool Input::ShouldGameObjectHandleInputEvent = true;
std::unordered_map<std::string, Input::Axis> Input::m_watchedAxisKeys;
std::unordered_map<std::string, f32> Input::m_axes;
bool Input::m_mouseLeft;
bool Input::m_mouseRight;
math::vec2 Input::m_mouseMove;
math::vec2 Input::m_mouseAcc;
bool Input::gamepadPresent = false;
float Input::axisDeadzone = 0.15f;
unsigned char Input::gamepadButtons[32];

void Input::setupBindings() {
    // Setup axes
    Input::createAxis("MoveForward", 
        GLFW_KEY_W, GLFW_KEY_S, 0, 0, 
        10, 12, -1, -1, 
        1, true);
    Input::createAxis("MoveRight", 
        GLFW_KEY_D, GLFW_KEY_A, 0, 0,
        11, 13, -1, -1,
        0);
    Input::createAxis("StrafeRight", 
        GLFW_KEY_E, GLFW_KEY_Q, 0, 0,
        5, 4, -1, -1,
        -1);
    Input::createAxis("Rotate", 
        GLFW_KEY_RIGHT, GLFW_KEY_LEFT, 0, 0, 
        -1, -1, -1, -1,
        -1, -1);

    // Setup keys
    Input::watchKey("key_w", GLFW_KEY_W);
    Input::watchKey("key_a", GLFW_KEY_A);
    Input::watchKey("key_s", GLFW_KEY_S);
    Input::watchKey("key_d", GLFW_KEY_D);
    Input::watchKey("key_space", GLFW_KEY_SPACE);
    Input::watchKey("key_shift", GLFW_KEY_LEFT_SHIFT);

    Input::watchKey("key_up", GLFW_KEY_UP);
    Input::watchKey("key_down", GLFW_KEY_DOWN);
    Input::watchKey("key_left", GLFW_KEY_LEFT);
    Input::watchKey("key_right", GLFW_KEY_RIGHT);
    Input::watchKey("key_numpad0", GLFW_KEY_KP_0);
    Input::watchKey("key_rctrl", GLFW_KEY_RIGHT_CONTROL);

    memset(gamepadButtons, 0, 32*sizeof(unsigned char));
}

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

bool Input::getLeftMouse() {
    return m_mouseLeft;
}

bool Input::getRightMouse() {
    return m_mouseRight;
}


void Input::watchKey(std::string key, int glfwKeyCode) {
    if (m_keyStates.find(key) == m_keyStates.end()) {
        // not already tracked

        m_keyStates[key] = false;
        m_watchedKeys[key] = glfwKeyCode;
    }
}

void Input::createAxis(std::string axisName,
    int glfwKeyCodePlus, int glfwKeyCodeMinus,
    int glfwKeyCodePlus_alt, int glfwKeyCodeMinus_alt,
    int glfwGamepadPlus, int glfwGamepadMinus,
    int glfwGamepadPlus_alt, int glfwGamepadMinus_alt,
    int glfwGamepadAxis, bool flipAxis) {

    if (m_axes.find(axisName) == m_axes.end()) {
        // not already tracked

        m_axes[axisName] = 0;
        m_watchedAxisKeys[axisName] = 
            Axis( 
                glfwKeyCodePlus , glfwKeyCodeMinus, 
                glfwKeyCodePlus_alt, glfwKeyCodeMinus_alt,
                glfwGamepadPlus, glfwGamepadMinus,
                glfwGamepadPlus_alt, glfwGamepadMinus_alt,
                glfwGamepadAxis, flipAxis );
    }
}

void Input::pollKeys(GLFWwindow* window, double dt) {
    // Update GamePad state
    int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
    if (present == 1 && !gamepadPresent) {
        gamepadPresent = true;
        Console::logMessage("Gamepad Connected");
    }
    else if (present == 0 && gamepadPresent) {
        gamepadPresent = false;
        Console::logMessage("Gamepad Disconnected");
    }

    // read gamepad states
    int numAxes;
    const float* axes;
    int numButtons;
    const unsigned char* buttons;
    if (gamepadPresent) {
        axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &numAxes);
        buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &numButtons);

        if (gamepadButtons) {
            for (int b = 0; b < numButtons; b++) {
                if (buttons[b] == GLFW_PRESS) {
                    // is currently down
                    if (gamepadButtons[b] == GLFW_PRESS) {
                        // is being held down
                    }
                    else if (gamepadButtons[b] == GLFW_RELEASE) {
                        // was just pressed
                    }
                }
                else if (buttons[b] == GLFW_RELEASE) {
                    // is currently up
                    if (gamepadButtons[b] == GLFW_PRESS) {
                        // was just released
                    }
                    else if (gamepadButtons[b] == GLFW_RELEASE) {
                        // is still released
                    }
                }

                gamepadButtons[b] = buttons[b];
            }
        }
    }

    // adjust internal representation of axes
    m_mouseMove = m_mouseAcc;
    m_mouseAcc = math::vec2();
    for (auto k : m_watchedKeys) {
        m_keyStates[k.first] = 
            (glfwGetKey(window, k.second) == GLFW_PRESS);
    }
    for (auto k : m_watchedAxisKeys) {
        auto key = k.first;
        auto axis = k.second;

        bool keyPlus =  (axis.kPlus    && glfwGetKey(window, axis.kPlus) == GLFW_PRESS) || 
                        (axis.kPlus_2  && glfwGetKey(window, axis.kPlus_2) == GLFW_PRESS);
        bool keyMinus = (axis.kMinus   && glfwGetKey(window, axis.kMinus) == GLFW_PRESS) ||
                        (axis.kMinus_2 && glfwGetKey(window, axis.kMinus_2) == GLFW_PRESS);

        auto keyValue = &m_axes[key];
        
        if (keyPlus || (gamepadPresent && (axis.gpPlus >= 0) && 
            (buttons[axis.gpPlus] == GLFW_PRESS || (buttons[axis.gpPlus_2] == GLFW_PRESS)))) {
            *keyValue = 1.0;
        }
        else if (keyMinus || (gamepadPresent && (axis.gpMinus >= 0) && 
            (buttons[axis.gpMinus] == GLFW_PRESS || buttons[axis.gpMinus_2] == GLFW_PRESS))) {
            *keyValue = -1.0;
        }
        else {
            *keyValue = 0.0;

            if (gamepadPresent) {
                if (axis.gpAxis >= 0 && axis.gpAxis < numAxes) {
                    float currAxis = axes[axis.gpAxis];

                    if (abs(currAxis) < axisDeadzone)
                        currAxis = 0;

                    *keyValue = axis.flipAxis ? -currAxis : currAxis;
                }
            }
        }
    }

    m_mouseLeft  = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    m_mouseRight = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

    if (gamepadPresent) {
        int axesCount;
        const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);

        int buttonCount;
        const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

        const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);

        /*std::cout << "Num of Axes: " << axesCount << std::endl;

        for (int n = 0; n < axesCount; n++) {
            std::cout << " " << n << ": " << axes[n];
        }

        if (GLFW_PRESS == buttons[1]) {
            std::cout << "A button is pressed\n";
        }
        else if (GLFW_RELEASE == buttons[1]) {
            std::cout << "B button is released\n";
        }

        std::cout << "Gamepad name: " << name << std::endl;
        */
    }
}

void Input::registerGameObject(GameObject* obj) {
    if (obj) {
        m_GameObjects.push_back(obj);
    }
}

void Input::setHandleInput(bool val) {
    ShouldGameObjectHandleInputEvent = val;
}