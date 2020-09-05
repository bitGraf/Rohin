#ifndef ENGINE_H_pp
#define ENGINE_H_pp

#include "Console.hpp"
#include "Utils.hpp"

/* Singleton Systems */
#include "Message/EMS.hpp"
#include "Resource/FileSystem.hpp"
#include "Resource/ResourceCatalog.hpp"
#include "Window/WindowManager.hpp"
#include "Resource/MemoryManager.hpp"
#include "Scene/SceneManager.hpp"

#include <thread>

const bool BUSY_LOOP = false;
const int FPS_NORMAL = 50;
const int FPS_HIGH = 250;

class Engine : public MessageReceiver {
public:
    Engine();

    bool Init(int argc, char* argv[]);
    void Start();

private:
    void End();
    void Update(double dt);
    void PreRender();
    void Render();

    bool done;

    double timeAcc, gameTime;

    /* Core System Singletons */
    EMS* ems;
    FileSystem* fileSystem;
    ResourceCatalog* catalog;
    WindowManager* windowManager;
    MemoryPool* memory;
    SceneManager* sceneManager;

    /* Frame Timing */
    using engine_clock = std::chrono::steady_clock;
    using Framerate = std::chrono::duration<engine_clock::rep, std::ratio<1, FPS_NORMAL>>;
    using Framerate_MAX = std::chrono::duration<engine_clock::rep, std::ratio<1, FPS_HIGH>>;
    MovingAverage<long long, FPS_NORMAL> fpsAvg;
    long long lastFrameTime;
    double dt;
    bool limitFramerate, highFramerate;
};

#endif
