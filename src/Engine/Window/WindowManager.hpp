#ifndef WINDOW_MANAGER_H_
#define WINDOW_MANAGER_H_

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <vector>

#include "Console.hpp"
#include "Message/EMS.hpp"
#include "Window/Window.hpp"
#include "Utils.hpp"

class WindowManager : public MessageReceiver {
public:
    static WindowManager* GetInstance();
    WindowManager();
    ~WindowManager();

    /* Initialization functions*/
    void update(double dt);
    bool Init();
    void Destroy();
    void SwapAndPoll();
    bool ShouldClose();

    Window window;

private:
    static WindowManager* _singleton;


    /* Error Callback */
    inline static void ErrorCallback(
        int error,
        const char* description) {

        Console::logError("GLFW Error: [%d] %s\n", error, description);
    }
};

#endif