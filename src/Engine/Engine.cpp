#include "Engine.hpp"


Engine::Engine() {
    done = false;
    userQuit = false;
}

void Engine::Start(handleMessageFnc f) {
    InitEngine(f);

    auto frameStart = engine_clock::now();
    auto frameEnd = frameStart + Framerate{ 1 };

    Console::logMessage("Starting message loop.");
    while (!done) {
        frameStart = engine_clock::now();

        lastFrameTime = std::chrono::duration_cast<std::chrono::microseconds>(
            frameStart - frameEnd).count();
        fpsAvg.addSample(lastFrameTime);
        dt = fpsAvg.getCurrentAverage() / 1000000.0;

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
            if (g_options.highFramerate)
                frameEnd = frameStart + Framerate_MAX{ 1 };
            else
                frameEnd = frameStart + Framerate{ 1 };

            // sleep until next loop starts
            if (BUSY_LOOP) {
                while (engine_clock::now() < frameEnd) {;}
            } else {
                std::this_thread::sleep_until(frameEnd);
            }
        }
        frameEnd = frameStart;
    }

    Console::logMessage("Game loop done, quitting...");

    End();
}

void Engine::InitEngine(handleMessageFnc f) {
    // Initialie the Game
    // Load ALL resources needed to run the game
    // Load the default scene

    MessageBus::create();
    MessageBus::setGlobalMessageHandleCallback(f);

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
    m_Renderer.renderBatch(&batch, fpsAvg.getCurrentAverage(), lastFrameTime);
}

void Engine::End() {
    glfwTerminate();

    Console::destroy();

    m_MainWindow.destroy();
    m_Renderer.destroy();
    m_Scenes.destroy();
    m_Resource.destroy();

    Console::rejoin();

    if (!userQuit)
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
}

void Engine::globalHandle(Message msg) {
    if (msg.isType("InputKey")) {
        using dt = Message::Datatype;
        dt key = msg.data[0];
        dt scancode = msg.data[1];
        dt action = msg.data[2];
        dt mods = msg.data[3];


        if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
            Console::logMessage("Switching game context");
        }

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            Console::logMessage("Quitting");
            userQuit = true;
            m_MainWindow.close();
        }
    }

    if (msg.isType("InputMouseButton")) {
        // int button, int action, int mods
        using dt = Message::Datatype;
        dt button = msg.data[0];
        dt action = msg.data[1];
        dt mods = msg.data[2];
        dt xPos = msg.data[3];
        dt yPos = msg.data[4];

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        }
    }
}