#ifndef ENGINE_H_pp
#define ENGINE_H_pp

#include "Console.hpp"
#include "Window/Window.hpp"
#include "Render/BatchRenderer.hpp"
#include "Render/DeferredBatchRenderer.hpp"
#include "Scene/Scene.hpp"
#include "OptionsPane.hpp"

#include <thread>

const bool BUSY_LOOP = true;
const int FPS_NORMAL = 50;
const int FPS_HIGH = 250;

class Engine {
public:
    Engine();

    void InitEngine(handleMessageFnc f, int argc, char* argv[]);
    void Start();
    void LoadLevel(std::string levelPath);
    void globalHandle(Message msg);

private:
    void End();
    void Update(double dt);
    void PreRender();
    void Render();

    bool done;
    bool userQuit;

    RenderBatch batch;

    /* Core Systems */
    Window m_MainWindow;
    //BatchRenderer m_Renderer;
    DeferredBatchRenderer m_Renderer;
    ResourceManager m_Resource;
    Camera m_debugCamera;
    //OptionsPane m_Options;

    /* State variables */
    bool debugMode;
    bool cursorMode;

    /* Loaded levels */
    std::vector<Scene*> m_scenes;

    /* Frame Timing */
    using engine_clock = std::chrono::steady_clock;
    using Framerate = std::chrono::duration<engine_clock::rep, std::ratio<1, FPS_NORMAL>>;
    using Framerate_MAX = std::chrono::duration<engine_clock::rep, std::ratio<1, FPS_HIGH>>;
    MovingAverage<long long, FPS_NORMAL> fpsAvg;
    long long lastFrameTime;
    double dt;
};

#endif
