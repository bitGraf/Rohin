#include "Engine.hpp"


Engine::Engine() {
    done = false;
    userQuit = false;
    cursorMode = true; //visible cursor
}

void Engine::Start() {
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

void Engine::InitEngine(handleMessageFnc f, int argc, char* argv[]) {
    // Initialie the Game
    // Load ALL resources needed to run the game
    // Load the default scene

    MessageBus::create();
    MessageBus::setGlobalMessageHandleCallback(f);

    m_Resource.setRootDirectory(argv[0]);
    MessageBus::registerSystem(m_Resource.create());
    MessageBus::registerSystem(m_MainWindow.create());
    MessageBus::registerSystem(m_Renderer.create());

    Message::listMessageTypes();

    EnvironmentMap::InitVAO();
    //m_Scenes.loadScenes(&m_Resource);
    m_Renderer.loadResources(&m_Resource);
    m_Options.create(&m_MainWindow);
    m_Options.redraw();

    m_MainWindow.makeCurrent();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(10);

    //m_debugCamera.playerControlled = true;
    m_MainWindow.cursorVisible(cursorMode);


    Input::watchKey("key_w", GLFW_KEY_W);
    Input::watchKey("key_a", GLFW_KEY_A);
    Input::watchKey("key_s", GLFW_KEY_S);
    Input::watchKey("key_d", GLFW_KEY_D);
    Input::watchKey("key_space", GLFW_KEY_SPACE);
    Input::watchKey("key_shift", GLFW_KEY_LEFT_SHIFT);

    Input::watchKey("key_up", GLFW_KEY_UP);
    Input::watchKey("key_down", GLFW_KEY_DOWN);
    Input::watchKey("key_left", GLFW_KEY_LEFT);
    Input::watchKey("key_right", GLFW_KEY_RIGHT);
    Input::watchKey("key_numpad0", GLFW_KEY_KP_0);
    Input::watchKey("key_rctrl", GLFW_KEY_RIGHT_CONTROL);

    m_debugCamera.Position = vec3(-4, 2, -4);
    m_debugCamera.YawPitchRoll = vec3(-45, -30, 0);
}

void Engine::LoadLevel(std::string levelPath) {
    // Load level
    DataBlock<Scene> k = m_Resource.reserveDataBlocks<Scene>(1);
    m_scenes.push_back(k.data);
    CurrentScene = k.data;
    CurrentScene->loadFromFile(&m_Resource, levelPath, false);
}

void Engine::Update(double dt) {
    // Don't send input events to GameObjects while in debug mode
    Input::setHandleInput(!debugMode);
    m_MainWindow.update(dt);

    if (debugMode) {
        m_debugCamera.Update(dt); // Only update the DebugCamera
    } else {
        if (GetScene())
            GetScene()->update(dt); // Update the whole scene
    }
}

void Engine::PreRender() {
    // Interrogate the SceneManager for a list of draw calls to make this frame.
    getRenderBatch(&batch);

    if (debugMode) {
        // inject debug camera values
        m_debugCamera.updateViewFrustum(m_MainWindow.m_width, m_MainWindow.m_height);
        batch.cameraViewProjectionMatrix = m_debugCamera.projectionMatrix *
            m_debugCamera.viewMatrix;
        batch.viewPos = m_debugCamera.Position;

        //batch.cameraView = m_debugCamera.viewMatrix;
        batch.cameraProjection = m_debugCamera.projectionMatrix;
    }
}

void Engine::Render() {
    m_Renderer.renderBatch(&batch);

    m_Renderer.renderDebug(
        &batch, fpsAvg.getCurrentAverage(), 
        lastFrameTime, debugMode);
}

void Engine::End() {
    glfwTerminate();

    m_MainWindow.destroy();
    m_Renderer.destroy();
    m_Resource.destroy();

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

        if (key == GLFW_KEY_C && action == GLFW_PRESS) {
            //m_debugCamera.playerControlled = cursorMode;
            cursorMode = !cursorMode;
            m_MainWindow.cursorVisible(cursorMode);
            m_debugCamera.freeFlyMode = !cursorMode;
        }

        if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
            Console::logMessage("Debug mode toggle");

            debugMode = !debugMode;

            if (debugMode) {
                glfwSetInputMode(m_MainWindow.m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
        }

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            Console::logMessage("Quitting");
            userQuit = true;
            m_MainWindow.close();
        }

        if (key == GLFW_KEY_BACKSLASH && action == GLFW_PRESS) {
            Console::logMessage("Reloading level");

            //m_currentScene->loadFromFile(&g_ResourceManager, "", false);
        }

        if (key == GLFW_KEY_R && action == GLFW_PRESS) {
            Console::logMessage("Camera Reset");
        }

        if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
            Console::logMessage("FPS Limit Toggle");

            if (g_options.limitFramerate) {
                if (g_options.highFramerate) {
                    g_options.limitFramerate = false;
                    g_options.highFramerate = false;
                }
                else {
                    g_options.highFramerate = true;
                }
            }
            else {
                g_options.limitFramerate = true;
            }
        }

        if (key == GLFW_KEY_O && action == GLFW_PRESS) {
            Console::logMessage("Toggle Options Pane");

            m_Options.ToggleVisibility();
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
            m_Options.click(xPos, yPos);
        }
    }

    if (GetScene()) {
        GetScene()->handleMessage(msg);
    }
}