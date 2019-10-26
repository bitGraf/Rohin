#include "Window.hpp"

Window::Window() {
    m_glfwWindow = nullptr;

    m_height = 600;
    m_width = 800;
}

void Window::update(double dt) {

}

void Window::handleMessage(Message msg) {

}

void Window::destroy() {
    //glfwDestroyWindow(m_glfwWindow);
    m_glfwWindow = nullptr;
}

void Window::sys_create(ConfigurationManager* configMgr) {
    create_window();
}

void Window::create_window(const char* title, int width, int height) {
    // Set window size
    m_width = width;
    m_height = height;

    // Register GLFW error callback first
    glfwSetErrorCallback(ErrorCallback);

    // intialize glfw
    glfwInit();

    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_glfwWindow = glfwCreateWindow(m_width, m_height, title, NULL, NULL);

    if (m_glfwWindow == NULL) {
        logMessage("Failed to create GLFW window. ");

        //m_console->logMessage("Failed to create GLFW window. ");
        glfwTerminate();
        // return NULL
    }

    makeCurrent();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        logMessage("Failed to initialize GLAD");
        // return NULL
    }

    // Set window position
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(m_glfwWindow, (int)(mode->width * .32), mode->height / 2 - m_height / 2);


    // set user pointer
    glfwSetWindowUserPointer(m_glfwWindow, this);

    // set our static functions as callbacks
    glfwSetWindowPosCallback(m_glfwWindow, WindowPosCallback);
    glfwSetWindowSizeCallback(m_glfwWindow, WindowResizeCallback);
    glfwSetWindowCloseCallback(m_glfwWindow, WindowQuitCallback);
    glfwSetWindowRefreshCallback(m_glfwWindow, WindowRefreshCallback);
    glfwSetWindowFocusCallback(m_glfwWindow, WindowFocusCallback);
    glfwSetWindowIconifyCallback(m_glfwWindow, WindowIconifyCallback);
    glfwSetWindowMaximizeCallback(m_glfwWindow, WindowMaximizeCallback);
    glfwSetFramebufferSizeCallback(m_glfwWindow, FramebufferSizeCallback);
    glfwSetWindowContentScaleCallback(m_glfwWindow, WindowContentScaleCallback);

    glfwSetKeyCallback(m_glfwWindow, InputKeyCallback);
    glfwSetCharCallback(m_glfwWindow, InputCharCallback);
    glfwSetCharModsCallback(m_glfwWindow, InputCharModsCallback);
    glfwSetMouseButtonCallback(m_glfwWindow, InputMouseButtonCallback);
    glfwSetCursorPosCallback(m_glfwWindow, InputCursorPosCallback);
    glfwSetCursorEnterCallback(m_glfwWindow, InputCursorEnterCallback);
    glfwSetScrollCallback(m_glfwWindow, InputScrollCallback);
    glfwSetDropCallback(m_glfwWindow, InputDropCallback);
}

void Window::makeCurrent() {
    glfwMakeContextCurrent(m_glfwWindow);
}

void Window::setPosition(int x, int y) {
    glfwSetWindowPos(m_glfwWindow, x, y);
}



/* Window Calback function */
void Window::WindowPositionUpdate(int xpos, int ypos) {
    //m_console->logMessage("Window::WindowPositionUpdate: ", 2, xpos, ypos);
}

void Window::WindowSizeUpdate(int w, int h) {
    putMessage(Message::Type::windowEvent, "WindowSizeUpdate");
}

void Window::WindowQuit() {
    logMessage("Window::WindowQuit");
    //destroy();
}

void Window::WindowRefresh() {
    logMessage("Window::WindowRefresh");
}

void Window::WindowFocusUpdate(int focused) {
    logMessage("Window::WindowFocusUpdate: ", 1, focused);
}

void Window::WindowIconifyUpdate(int iconify) {
    logMessage("Window::WindowIconifyUpdate: ", 1, iconify);
}

void Window::WindowMaximizeUpdate(int maximize) {
    logMessage("Window::WindowMaximizeUpdate: ", 1, maximize);
}

void Window::WindowFramebufferUpdate(int w, int h) {
    logMessage("Window::WindowFramebufferUpdate: ", 2, w, h);
    //glViewport(0, 0, w, h);
}

void Window::WindowScaleUpdate(float xscale, float yscale) {
    //m_console->logMessage("Window::WindowScaleUpdate: ", 2, xscale, yscale);
    //m_console->logMessage("Window::WindowScaleUpdate: _, _");
}

/* Input Callback Function */
void Window::InputKey(int key, int scancode, int action, int mods) {
    //logMessage("Window::InputKey: ", 4, key, scancode, action, mods);

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        logMessage("Spacebar pressed");
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        logMessage("P pressed");
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS && !(mods & GLFW_MOD_SHIFT)) {
        putMessage(Message::Type::renderEvent, "startDraw");
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS && (mods & GLFW_MOD_SHIFT)) {
        putMessage(Message::Type::renderEvent, "endDraw");
    }
}

void Window::InputChar(unsigned int codepoint) {
    //m_console->logMessage("Window::InputChar: ", 1, (int)codepoint);
}

void Window::InputCharMod(unsigned int codepoint, int mods) {
    //m_console->logMessage("Window::InputCharMod: ", 2, (int)codepoint, mods);
}

void Window::InputMouseButton(int button, int action, int mods) {
    //m_console->logMessage("Window::InputMouseButton: ", 3, button, action, mods);
}

void Window::InputCursorPos(double xpos, double ypos) {
    //m_console->logMessage("Window::InputCursorPos: ", 2, xpos, ypos);
    //m_console->logMessage("Window::InputCursorPos: _, _");
}

void Window::InputCursorEnter(int entered) {
    //m_console->logMessage("Window::InputCursorEnter: ", 1, entered);
}

void Window::InputScroll(double xoffset, double yoffset) {
    //m_console->logMessage("Window::InputScroll: ", 2, xoffset, yoffset);
    //m_console->logMessage("Window::InputScroll: _, _");
}

void Window::InputDrop(int count, const char** paths) {
    logMessage("Window::InputDrop: ", 1, count);
    logMessage(paths[0]);
}


bool Window::shouldClose() {
    return glfwWindowShouldClose(m_glfwWindow);
}

void Window::swapAndPoll() {
    glfwSwapBuffers(m_glfwWindow);
    glfwPollEvents();
}