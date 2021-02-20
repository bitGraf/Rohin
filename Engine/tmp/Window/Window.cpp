#include "rhpch.hpp"
#include "Window.hpp"

Window::Window() {
    m_glfwWindow = nullptr;

    m_height = 800;
    m_width = 600;
    cursorMode = false;
    hasResized = true;
}

void Window::update(double dt) {
    if (hasResized) {
        LOG_INFO("New window size: %d x %d.", m_width, m_height);
        hasResized = false;
    }
}

void Window::Destroy() {
    //glfwDestroyWindow(m_glfwWindow);
    m_glfwWindow = nullptr;
}

bool Window::Init(const char* title, int width, int height, bool hidden) {
    // Set window size
    m_width = width;
    m_height = height;
    cursorOver = false;

    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, hidden ? GLFW_FALSE : GLFW_TRUE);
    m_glfwWindow = glfwCreateWindow(m_width, m_height, title, NULL, NULL);

    if (m_glfwWindow == NULL) {
        LOG_ERROR("Failed to create GLFW window. ");

        //m_console->logMessage("Failed to create GLFW window. ");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_glfwWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG_ERROR("Failed to initialize GLAD");
        return false;
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

    glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, cursorMode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    return true;
}

void Window::makeCurrent() {
    glfwMakeContextCurrent(m_glfwWindow);
    //sendMessage("WindowContext");
    glViewport(0, 0, m_width, m_height);
}

void Window::setPosition(int x, int y) {
    glfwSetWindowPos(m_glfwWindow, x, y);
}

void Window::resize(int width, int height) {
    glfwSetWindowSize(m_glfwWindow, width, height);
}

void Window::cursorVisible(bool vis) {
    cursorMode = vis;

    glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, 
        cursorMode ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}



/* Window Calback functions */
void Window::WindowPositionUpdate(int xpos, int ypos) {
}

void Window::WindowSizeUpdate(int w, int h) {
}

void Window::WindowQuit() {
}

void Window::WindowRefresh() {
}

void Window::WindowFocusUpdate(int focused) {
}

void Window::WindowIconifyUpdate(int iconify) {
}

void Window::WindowMaximizeUpdate(int maximize) {
}

void Window::WindowFramebufferUpdate(int w, int h) {
    m_width = w;
    m_height = h;
    glViewport(0, 0, m_width, m_height);

    hasResized = true;
}

void Window::WindowScaleUpdate(float xscale, float yscale) {
}

/* Input Callback Function */
void Window::InputKey(int key, int scancode, int action, int mods) {
}

void Window::InputChar(unsigned int codepoint) {
}

void Window::InputCharMod(unsigned int codepoint, int mods) {
}

void Window::InputMouseButton(int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(m_glfwWindow, &x, &y);
    s32 xpos = static_cast<s32>(floor(x));
    s32 ypos = static_cast<s32>(floor(y));
}

void Window::InputCursorPos(double xpos, double ypos) {
    math::vec2 delta = math::vec2(xpos, ypos) - m_cursorPos;
    Input::m_mouseAcc += delta;
    m_cursorPos = math::vec2(xpos, ypos);
}

void Window::InputCursorEnter(int entered) {
    cursorOver = (entered == 1);
}

void Window::InputScroll(double xoffset, double yoffset) {
}

void Window::InputDrop(int count, const char** paths) {
    LOG_INFO("Window::InputDrop: ", 1, count);
    LOG_INFO(paths[0]);
}


bool Window::shouldClose() {
    return glfwWindowShouldClose(m_glfwWindow);
}

void Window::close() {
    glfwSetWindowShouldClose(m_glfwWindow, true);
}

void Window::swapAndPoll() {
    glfwSwapBuffers(m_glfwWindow);
    glfwPollEvents();
}

math::vec2 Window::getCursorPos() {
    double x, y;
    glfwGetCursorPos(m_glfwWindow, &x, &y);
    return math::vec2(x, y);
}
