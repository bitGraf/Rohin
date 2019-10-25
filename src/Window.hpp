#ifndef WINDOW_H_
#define WINDOW_H_

#include <GLFW\glfw3.h>
#include <glad\glad.h>

#include "CoreSystem.hpp"

#define WINDOW_DEFAULT_HEIGHT   600
#define WINDOW_DEFAULT_WIDTH    800

/// acting as WindowManager for now
class Window : public CoreSystem {
public:
    Window();

    /* Initialization functions*/
    void update(double dt);
    void handleMessage(Message msg);
    void destroy();
    void sys_create(ConfigurationManager* configMgr);

    void create_window(const char* title = "Window", int width = WINDOW_DEFAULT_WIDTH, int height = WINDOW_DEFAULT_HEIGHT);

    /* Window functions */
    bool shouldClose();
    void swapAndPoll();
    void makeCurrent();
    void setPosition(int x, int y);
    
    /* Callback responses */
    void WindowPositionUpdate(int xpos, int ypos);
    void WindowSizeUpdate(int w, int h);
    void WindowQuit();
    void WindowRefresh();
    void WindowFocusUpdate(int focused);
    void WindowIconifyUpdate(int iconify);
    void WindowMaximizeUpdate(int maximize);
    void WindowFramebufferUpdate(int w, int h);
    void WindowScaleUpdate(float xscale, float yscale);

    void InputKey(int key, int scancode, int action, int mods);
    void InputChar(unsigned int codepoint);
    void InputCharMod(unsigned int codepoint, int mods);
    void InputMouseButton(int button, int action, int mods);
    void InputCursorPos(double xpos, double ypos);
    void InputCursorEnter(int entered);
    void InputScroll(double xoffset, double yoffset);
    void InputDrop(int count, const char** paths);

private:
    GLFWwindow*     m_glfwWindow;

    // Window properties
    int m_height;
    int m_width;

    /* Error Callback */
    inline static void ErrorCallback(
        int error,
        const char* description) {

        //TODO change to console log?
        printf("GLFW Error: [%d] %s\n", error, description);
    }
    /* Window Callbacks */
    inline static void WindowPosCallback(
        GLFWwindow* win,
        int xpos,
        int ypos) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->WindowPositionUpdate(xpos, ypos);
    }
    inline static void WindowResizeCallback(
        GLFWwindow* win,
        int w,
        int h) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->WindowSizeUpdate(w, h);
    }
    inline static void WindowQuitCallback(
        GLFWwindow* win) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->WindowQuit();
    }
    inline static void WindowRefreshCallback(
        GLFWwindow* win) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->WindowRefresh();
    }
    inline static void WindowFocusCallback(
        GLFWwindow* win,
        int focused) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->WindowFocusUpdate(focused);
    }
    inline static void WindowIconifyCallback(
        GLFWwindow* win,
        int iconify) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->WindowIconifyUpdate(iconify);
    }
    inline static void WindowMaximizeCallback(
        GLFWwindow* win,
        int maximize) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->WindowMaximizeUpdate(maximize);
    }
    inline static void FramebufferSizeCallback(
        GLFWwindow* win,
        int w,
        int h) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->WindowFramebufferUpdate(w, h);
    }
    inline static void WindowContentScaleCallback(
        GLFWwindow* win,
        float xscale,
        float yscale) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->WindowScaleUpdate(xscale, yscale);
    }

    /* Input Callbacks */

    inline static void InputKeyCallback(
        GLFWwindow* win,
        int key,
        int scancode,
        int action,
        int mods) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->InputKey(key, scancode, action, mods);
    }
    inline static void InputCharCallback(
        GLFWwindow* win,
        unsigned int codepoint) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->InputChar(codepoint);
    }
    inline static void InputCharModsCallback(
        GLFWwindow* win,
        unsigned int codepoint,
        int mods) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->InputCharMod(codepoint, mods);
    }
    inline static void InputMouseButtonCallback(
        GLFWwindow* win,
        int button,
        int action,
        int mods) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->InputMouseButton(button, action, mods);
    }
    inline static void InputCursorPosCallback(
        GLFWwindow* win,
        double xpos,
        double ypos) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->InputCursorPos(xpos, ypos);
    }
    inline static void InputCursorEnterCallback(
        GLFWwindow* win,
        int entered) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->InputCursorEnter(entered);
    }
    inline static void InputScrollCallback(
        GLFWwindow* win,
        double xoffset,
        double yoffset) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->InputScroll(xoffset, yoffset);
    }
    inline static void InputDropCallback(
        GLFWwindow* win,
        int count,
        const char** paths) {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->InputDrop(count, paths);
    }
};

#endif


/*
Window Callbacks

glfwSetWindowPosCallback            (GLFWwindow *window, GLFWwindowposfun cbfun)
glfwSetWindowSizeCallback           (GLFWwindow *window, GLFWwindowsizefun cbfun)
glfwSetWindowCloseCallback          (GLFWwindow *window, GLFWwindowclosefun cbfun)
glfwSetWindowRefreshCallback        (GLFWwindow *window, GLFWwindowrefreshfun cbfun)
glfwSetWindowFocusCallback          (GLFWwindow *window, GLFWwindowfocusfun cbfun)
glfwSetWindowIconifyCallback        (GLFWwindow *window, GLFWwindowiconifyfun cbfun)
glfwSetWindowMaximizeCallback       (GLFWwindow *window, GLFWwindowmaximizefun cbfun)
glfwSetFramebufferSizeCallback      (GLFWwindow *window, GLFWframebuffersizefun cbfun)
glfwSetWindowContentScaleCallback   (GLFWwindow *window, GLFWwindowcontentscalefun cbfun)

*/

/*
Input Callbacks

glfwSetKeyCallback          (GLFWwindow *window, GLFWkeyfun cbfun)
glfwSetCharCallback         (GLFWwindow *window, GLFWcharfun cbfun)
glfwSetCharModsCallback     (GLFWwindow *window, GLFWcharmodsfun cbfun)
glfwSetMouseButtonCallback  (GLFWwindow *window, GLFWmousebuttonfun cbfun)
glfwSetCursorPosCallback    (GLFWwindow *window, GLFWcursorposfun cbfun)
glfwSetCursorEnterCallback  (GLFWwindow *window, GLFWcursorenterfun cbfun)
glfwSetScrollCallback       (GLFWwindow *window, GLFWscrollfun cbfun)
glfwSetDropCallback         (GLFWwindow *window, GLFWdropfun cbfun)

*/