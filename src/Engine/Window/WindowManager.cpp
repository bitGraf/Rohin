#include "WindowManager.hpp"

WindowManager::WindowManager() {}
WindowManager::~WindowManager() {}

WindowManager* WindowManager::_singleton = 0;

WindowManager* WindowManager::GetInstance() {
    if (!_singleton) {
        _singleton = new WindowManager;
    }
    return _singleton;
}

void WindowManager::Destroy() {
    window.Destroy();

    if (_singleton) {
        delete _singleton;
        _singleton = 0;
    }
}

bool WindowManager::Init() {
    // Register GLFW error callback first
    glfwSetErrorCallback(ErrorCallback);

    // intialize glfw
    if (glfwInit() == GLFW_FALSE) {
        Console::logError("Failed to initialize GLFW");
        return false;
    }

    window.Init("Main Window", 800, 600);
    window.makeCurrent();

    return true;
}

void WindowManager::update(double dt) {
    window.update(dt);
}

void WindowManager::SwapAndPoll() {
    window.swapAndPoll();
}

bool WindowManager::ShouldClose() {
    return window.shouldClose();
}