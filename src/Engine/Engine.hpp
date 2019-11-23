#ifndef ENGINE_H_pp
#define ENGINE_H_pp

#include "Console.hpp"
#include "Window\Window.hpp"
#include "Render\NewRender.hpp"
#include "Scene\SceneManager.hpp"

class Engine {
public:
    Engine();

    void Start();

private:
    void InitEngine();
    void End();
    void Update(double dt);
    void PreRender();
    void Render();

    bool done;

    BatchDrawCall batch;

    /* Core Systems */
    Window m_MainWindow;
    BatchRenderer m_Renderer;
    SceneManager m_Scenes;
    ResourceManager m_Resource;

    /* Frame Timing */
    using _clock = std::chrono::system_clock;
    using _time = std::chrono::system_clock::time_point;
};

#endif
