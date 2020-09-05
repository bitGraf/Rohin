#include "Engine.hpp"


Engine::Engine() {
    done = false;
    limitFramerate = true;
    highFramerate = false;

    ems = 0;
    fileSystem = 0;
    catalog = 0;
    windowManager = 0;
    sceneManager = 0;

    timeAcc = 0;
    gameTime = 0;
}

bool Engine::Init(int argc, char* argv[]) {
    /* Perform system initializations */
    ems = EMS::GetInstance();
    if (!ems || !ems->Init()) {
        Console::logError("Failed to create EMS instance");
        return false;
    }

    fileSystem = FileSystem::GetInstance();
    if (!fileSystem || !fileSystem->Init(argv[0])) {
        Console::logError("Failed to create FileSystem instance");
        return false;
    }

    memory = MemoryPool::GetInstance();
    if (!memory || !memory->Init()) {
        Console::logError("Failed to create MemoryPool instance");
        return false;
    }

    catalog = ResourceCatalog::GetInstance();
    if (!catalog || !catalog->Init()) {
        Console::logError("Failed to create ResourceCatalog instance");
        return false;
    }

    windowManager = WindowManager::GetInstance();
    if (!windowManager || !windowManager->Init()) {
        Console::logError("Failed to create WindowManager instance");
        return false;
    }

    sceneManager = SceneManager::GetInstance();
    if (!sceneManager || !sceneManager->Init()) {
        Console::logError("Failed to create SceneManager instance");
        return false;
    }

    /* free to use systems now */
    catalog->createNewResource("Data/Shaders/pipeline/ssao.frag", resShader, true);
    fileSystem->checkForFileUpdates();

    catalog->loadResourceFile("level.mcf");

    sceneManager->AddSceneToList("Data/Levels/test.scene");

    return true;
}

void Engine::Start() {
    auto frameStart = engine_clock::now();
    auto frameEnd = frameStart + Framerate{ 1 };

    while (!done) {
        frameStart = engine_clock::now();

        lastFrameTime = std::chrono::duration_cast<std::chrono::microseconds>(
            frameStart - frameEnd).count();
        fpsAvg.addSample(lastFrameTime);
        dt = fpsAvg.getCurrentAverage() / 1000000.0;

        if (!ems->Exec()) {
            Console::logError("Failed to process event queue");
            done = true;
            break;
        }

        if (windowManager->ShouldClose()) {
            Console::logMessage("Main window closed");
            done = true;
            break;
        }

        Update(dt);

        PreRender();

        Render();

        if (limitFramerate) {
            if (highFramerate)
                frameEnd = frameStart + Framerate_MAX{ 1 };
            else
                frameEnd = frameStart + Framerate{ 1 };

            // sleep until next loop starts
            if (BUSY_LOOP) {
                while (engine_clock::now() < frameEnd) { ; }
            }
            else {
                std::this_thread::sleep_until(frameEnd);
            }
        }
        frameEnd = frameStart;
    }

    End();
}

void Engine::Update(double dt) {
    // Don't send input events to GameObjects while in debug mode
    gameTime += dt;
    timeAcc += dt;

    /* Every 5 seconds poll all the files watched */
    if (timeAcc > 5.0) {
        timeAcc -= 5.0;

        Console::logMessage("Checking Files");
        Post(eveCheckFileMod);
    }
}

void Engine::PreRender() {
    // Interrogate the SceneManager for a list of draw calls to make this frame.
}

void Engine::Render() {
    windowManager->SwapAndPoll();
}

void Engine::End() {
    ems->Destroy();
    fileSystem->Destroy();
    windowManager->Destroy();

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
}