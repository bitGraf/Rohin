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
        Input::pollKeys(m_MainWindow.m_glfwWindow, dt);

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
    //m_MainWindow.resize(1200, 900);

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

    Message::registerMessageType("CollisionEvent");

    m_debugCamera.Position = vec3(-4, 2, -4);
    m_debugCamera.YawPitchRoll = vec3(-45, -30, 0);

    // Load collision info
    //cWorld.testCreate(&m_Resource);
    UID_t floor = cWorld.CreateNewCubeHull(&m_Resource, vec3(0, -2.5, 0), 75, 5, 75);
    cWorld.CreateNewCubeHull(&m_Resource, vec3(0, 1.5, -4), 8, 3, 3);
    UID_t id = cWorld.CreateNewCubeHull(&m_Resource, vec3(-5.42, 1, 1.88), 2);
    cWorld.getHullFromID(id)->rotation.toYawPitchRoll(38.27, 0, 0);
    cWorld.CreateNewCubeHull(&m_Resource, vec3(-3.41, 0.5, -0.89), .75, 1.5, .75);
    cWorld.CreateNewCubeHull(&m_Resource, vec3(-1.47, 0.5, 2.09), .75, 1.5, .75);
    UID_t ramp = cWorld.CreateNewCubeHull(&m_Resource, vec3(5, 0, 5), 10, 1, 4);
    cWorld.getHullFromID(ramp)->rotation.toYawPitchRoll(0, 10, 0);
}

void Engine::LoadLevel(std::string levelPath) {
    // Load level
    DataBlock<Scene> k = m_Resource.reserveDataBlocks<Scene>(1);
    m_scenes.push_back(k.data);
    SetCurrentScene(k.data);
    GetScene()->loadFromFile(&m_Resource, levelPath, false);
}

void Engine::Update(double dt) {
    // Don't send input events to GameObjects while in debug mode
    Input::setHandleInput(!debugMode);
    m_MainWindow.update(dt);
    m_Renderer.update(dt);

    if (debugMode) {
        m_debugCamera.Update(dt); // Only update the DebugCamera
    } else {
        float tf;
        if (glfwGetKey(m_MainWindow.m_glfwWindow, GLFW_KEY_LEFT_SHIFT)) {
            tf = 0.25;
        }
        else {
            tf = 1;
        }
        if (GetScene())
            GetScene()->update(.02 * tf); // Update the whole scene
    }
}

void Engine::PreRender() {
    // Interrogate the SceneManager for a list of draw calls to make this frame.
    getRenderBatch(&batch, !debugMode);

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
            switch (m_debugCamera.m_cameraMode) {
                case Camera::eCameraMode::Static: {
                    m_debugCamera.m_cameraMode = Camera::eCameraMode::Freefly;
                } break;
                case Camera::eCameraMode::Freefly: {
                    m_debugCamera.m_cameraMode = Camera::eCameraMode::Static;
                } break;
            }
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

            LoadLevel("Data/outFile.scene");
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

            if (GLFW_CURSOR_NORMAL == glfwGetInputMode(m_MainWindow.m_glfwWindow, GLFW_CURSOR)) {
                auto id = m_Renderer.pickObject(xPos, yPos);
                GameObject* go = GetScene()->getObjectByID(id);
                if (go) {
                    Console::logMessage("You clicked on GameObject id [" + std::to_string(id) + "]:{" + go->Name + "}");
                }
            }
        }
    }

    if (GetScene()) {
        GetScene()->handleMessage(msg);
    }
}