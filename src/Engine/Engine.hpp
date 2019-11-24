#ifndef ENGINE_H_pp
#define ENGINE_H_pp

#include "Console.hpp"
#include "Window\Window.hpp"
#include "Render\RenderManager.hpp"
#include "Scene\SceneManager.hpp"

const bool BUSY_LOOP = true;
const int FPS_NORMAL = 50;
const int FPS_HIGH = 250;

class Engine {
public:
    Engine();

    void Start(handleMessageFnc f);
    void globalHandle(Message msg);

private:
    void InitEngine(handleMessageFnc f);
    void End();
    void Update(double dt);
    void PreRender();
    void Render();

    bool done;
    bool userQuit;

    BatchDrawCall batch;

    /* Core Systems */
    Window m_MainWindow;
    BatchRenderer m_Renderer;
    SceneManager m_Scenes;
    ResourceManager m_Resource;
    Camera m_debugCamera;
    bool debugMode;
    bool cursorMode;

    /* Frame Timing */
    using engine_clock = std::chrono::steady_clock;
    using Framerate = std::chrono::duration<engine_clock::rep, std::ratio<1, FPS_NORMAL>>;
    using Framerate_MAX = std::chrono::duration<engine_clock::rep, std::ratio<1, FPS_HIGH>>;
    MovingAverage<long long, FPS_NORMAL> fpsAvg;
    long long lastFrameTime;
    double dt;
};

#endif
