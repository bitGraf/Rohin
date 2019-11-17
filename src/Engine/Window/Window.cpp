#include "Window.hpp"

Window::Window() {
    m_glfwWindow = nullptr;

    m_height = WINDOW_DEFAULT_HEIGHT;
    m_width = WINDOW_DEFAULT_WIDTH;
    cursorMode = false;
}

void Window::update(double dt) {

}

void Window::handleMessage(Message msg) {
    if (msg.isType("InputKey")) {
        // int button, int action, int mods
        using dt = Message::Datatype;
        dt key = msg.data[0];
        dt scancode = msg.data[1];
        dt action = msg.data[2];
        dt mods = msg.data[3];

        if (key == GLFW_KEY_C && action == GLFW_PRESS) {
            cursorMode = !cursorMode;
            glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, cursorMode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
    }
}

void Window::destroy() {
    //glfwDestroyWindow(m_glfwWindow);
    m_glfwWindow = nullptr;
}

CoreSystem* Window::create() {
    Message::registerMessageType("WindowCreate");
    Message::registerMessageType("WindowMove");
    Message::registerMessageType("WindowFocus");
    Message::registerMessageType("WindowResize");
    Message::registerMessageType("WindowContext");
    Message::registerMessageType("WindowClose");

    Message::registerMessageType("InputKey");
    Message::registerMessageType("InputMouseButton");
    Message::registerMessageType("InputMouseMove");
    Message::registerMessageType("InputCursorEnter");
    Message::registerMessageType("InputScroll");

    Message::registerMessageType("FileDrop");

    create_window();

    glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, cursorMode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

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
        Console::logMessage("Failed to create GLFW window. ");

        //m_console->logMessage("Failed to create GLFW window. ");
        glfwTerminate();
        return;
    }

    makeCurrent();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Console::logMessage("Failed to initialize GLAD");
        return;
    }

    sendMessage(Message("WindowCreate"));

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


    sendMessage(Message("WindowMove", 2, xpos, ypos));
}

void Window::WindowSizeUpdate(int w, int h) {
}

void Window::WindowQuit() {
    //destroy();
    sendMessage(Message("WindowClose"));
}

void Window::WindowRefresh() {
}

void Window::WindowFocusUpdate(int focused) {
    sendMessage(Message("WindowFocus", 1, focused));
}

void Window::WindowIconifyUpdate(int iconify) {
}

void Window::WindowMaximizeUpdate(int maximize) {
}

void Window::WindowFramebufferUpdate(int w, int h) {
    m_width = w;
    m_height = h;
    glViewport(0, 0, m_width, m_height);

    sendMessage(Message("WindowResize", 2, w, h));
}

void Window::WindowScaleUpdate(float xscale, float yscale) {
}

/* Input Callback Function */
void Window::InputKey(int key, int scancode, int action, int mods) {
    sendMessage(Message("InputKey", 4, key, scancode, action, mods));

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        Console::logMessage("Spacebar pressed");
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        Console::logMessage("P pressed");
    }
}

void Window::InputChar(unsigned int codepoint) {
}

void Window::InputCharMod(unsigned int codepoint, int mods) {
}

void Window::InputMouseButton(int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(m_glfwWindow, &x, &y);
    using dt = Message::Datatype;
    dt xpos = static_cast<dt>(floor(x));
    dt ypos = static_cast<dt>(floor(y));
    sendMessage(Message("InputMouseButton", 5, button, action, mods, xpos, ypos));
}

void Window::InputCursorPos(double xpos, double ypos) {
    //sendMessage("InputMouseMove");
    math::vec2 delta = math::vec2(xpos, ypos) - m_cursorPos;
    Input::m_mouseAcc += delta;
    m_cursorPos = math::vec2(xpos, ypos);
}

void Window::InputCursorEnter(int entered) {
    sendMessage(Message("InputCursorEnter"));
}

void Window::InputScroll(double xoffset, double yoffset) {
}

void Window::InputDrop(int count, const char** paths) {
    //logMessage("Window::InputDrop: ", 1, count);
    //logMessage(paths[0]);
    sendMessage(Message("FileDrop"));
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