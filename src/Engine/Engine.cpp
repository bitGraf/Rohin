#include "Engine.hpp"


Engine::Engine() {
    done = false;
}

void Engine::Start() {
    InitEngine();

    double dt = .02;

    _time frameEnd = _clock::now();
    long long microsPerFrame = 20000; // 50 fps

    Console::logMessage("Starting message loop.");
    while (!done) {
        frameEnd = _clock::now() + std::chrono::microseconds(microsPerFrame);

        if (m_MainWindow.shouldClose()) {
            done = true;
            break;
        }

        MessageBus::processEntireQueue();
        Input::pollKeys(m_MainWindow.m_glfwWindow);

        Update(dt);

        PreRender();

        Render();

        m_MainWindow.swapAndPoll();

        if (g_options.limitFramerate) {
            std::this_thread::sleep_until(frameEnd);
        }
    }

    Console::logMessage("Game loop done, quitting...");

    End();
}

void Engine::InitEngine() {
    // Initialie the Game
    // Load ALL resources needed to run the game
    // Load the default scene

    MessageBus::create();
    //MessageBus::setGlobalMessageHandleCallback(Engine::hm);

    MessageBus::registerSystem(m_Resource.create());
    MessageBus::registerSystem(m_MainWindow.create());
    MessageBus::registerSystem(m_Renderer.create());
    MessageBus::registerSystem(m_Scenes.create());

    Message::listMessageTypes();

    EnvironmentMap::InitVAO();
    m_Scenes.loadScenes(&m_Resource);

    Console::startListening(false);

    m_MainWindow.makeCurrent();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(10);
}

void Engine::Update(double dt) {
    // Call the SceneManager update function
    // Update the Game State
    m_Scenes.update(dt);
}

void Engine::PreRender() {
    // Interrogate the SceneManager for a list of draw calls to make this frame.
    m_Scenes.getRenderBatch(&batch);
}

void Engine::Render() {
    m_Renderer.renderBatch(&batch);
}

void Engine::End() {
    glfwTerminate();

    Console::destroy();

    m_MainWindow.destroy();
    m_Renderer.destroy();
    m_Scenes.destroy();
    m_Resource.destroy();

    Console::rejoin();


    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
}