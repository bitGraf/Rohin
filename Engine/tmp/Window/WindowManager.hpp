#ifndef WINDOW_MANAGER_H_
#define WINDOW_MANAGER_H_

#include <vector>

#include "Engine/Core/Logger.hpp"
#include "Engine/Event/Event.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/Utils.hpp"

class WindowManager {
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

        LOG_ERROR("GLFW Error: [%d] %s\n", error, description);
    }
};

#endif