#ifndef ENGINE_H_pp
#define ENGINE_H_pp

#include "Console.hpp"
#include "Window\Window.hpp"
#include "Render\NewRender.hpp"
#include "Scene\SceneManager.hpp"

const bool BUSY_LOOP = true;

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

    /* Frame Timing */
    using engine_clock = std::chrono::steady_clock;
    using Framerate = std::chrono::duration<engine_clock::rep, std::ratio<1, 50>>;
    using Framerate_MAX = std::chrono::duration<engine_clock::rep, std::ratio<1, 250>>;
    MovingAverage<long long, 50> fpsAvg;
    long long lastFrameTime;
    double dt;
};

#endif
