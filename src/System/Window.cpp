#include "Window.hpp"

Window::Window() {
    m_glfwWindow = nullptr;

    m_height = WINDOW_DEFAULT_HEIGHT;
    m_width = WINDOW_DEFAULT_WIDTH;
}

void Window::update(double dt) {

}

void Window::handleMessage(Message msg) {

}

void Window::destroy() {
    //glfwDestroyWindow(m_glfwWindow);
    m_glfwWindow = nullptr;
}

CoreSystem* Window::create() {
    Configuration::registerMessageType("WindowCreate");
    Configuration::registerMessageType("WindowMove");
    Configuration::registerMessageType("WindowFocus");
    Configuration::registerMessageType("WindowResize");
    Configuration::registerMessageType("WindowContext");
    Configuration::registerMessageType("WindowClose");

    Configuration::registerMessageType("InputKey");
    Configuration::registerMessageType("InputMouseButton");
    Configuration::registerMessageType("InputMouseMove");
    Configuration::registerMessageType("InputCursorEnter");
    Configuration::registerMessageType("InputScroll");

    Configuration::registerMessageType("FileDrop");

    create_window();

    return this;
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
        return;
    }

    makeCurrent();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        logMessage("Failed to initialize GLAD");
        return;
    }

    sendMessage("WindowCreate");

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
    //sendMessage("WindowContext");
}

void Window::setPosition(int x, int y) {
    glfwSetWindowPos(m_glfwWindow, x, y);
}

void Window::resize(int width, int height) {
    glfwSetWindowSize(m_glfwWindow, width, height);
}



/* Window Calback functions */
void Window::WindowPositionUpdate(int xpos, int ypos) {
    // TODO explore this possibility more
    //Message msg = Configuration::encodeData("WindowMove", xpos, ypos);
    //sendMessage(msg);


    sendMessage("WindowMove");
}

void Window::WindowSizeUpdate(int w, int h) {
}

void Window::WindowQuit() {
    //destroy();
    sendMessage("WindowClose");
}

void Window::WindowRefresh() {
}

void Window::WindowFocusUpdate(int focused) {
    sendMessage("WindowFocus");
}

void Window::WindowIconifyUpdate(int iconify) {
}

void Window::WindowMaximizeUpdate(int maximize) {
}

void Window::WindowFramebufferUpdate(int w, int h) {
    m_width = w;
    m_height = h;
    glViewport(0, 0, m_width, m_height);

    sendMessage("WindowResize");
}

void Window::WindowScaleUpdate(float xscale, float yscale) {
}

/* Input Callback Function */
void Window::InputKey(int key, int scancode, int action, int mods) {
    sendMessage("InputKey");

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        logMessage("Spacebar pressed");
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        logMessage("P pressed");
    }
}

void Window::InputChar(unsigned int codepoint) {
}

void Window::InputCharMod(unsigned int codepoint, int mods) {
}

void Window::InputMouseButton(int button, int action, int mods) {
    sendMessage("InputMouseButton");
}

void Window::InputCursorPos(double xpos, double ypos) {
    //sendMessage("InputMouseMove");
}

void Window::InputCursorEnter(int entered) {
    sendMessage("InputCursorEnter");
}

void Window::InputScroll(double xoffset, double yoffset) {
}

void Window::InputDrop(int count, const char** paths) {
    //logMessage("Window::InputDrop: ", 1, count);
    //logMessage(paths[0]);
    sendMessage("FileDrop");
}


bool Window::shouldClose() {
    return glfwWindowShouldClose(m_glfwWindow);
}

void Window::swapAndPoll() {
    glfwSwapBuffers(m_glfwWindow);
    glfwPollEvents();
}