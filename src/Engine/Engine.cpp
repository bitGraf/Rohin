#include "Engine.hpp"


Engine::Engine() {
    done = false;
    limitFramerate = true;
    highFramerate = false;
    runtimeProfile = false;

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

    // Put after FSys init so path is correct
    BENCHMARK_START_SESSION("Initialization", "init.json");

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

    //sceneManager->LoadNewScene("Data/Levels/outFile.scene");
    //sceneManager->LoadNewScene("Data/Levels/sceneHeirarchy.scene");
    sceneManager->LoadNewScene("Data/Levels/test.scene");

    /* print out memory usage */
    u32 left = memory->getBytesLeft();
    u32 total = memory->getTotalBytes();
    Console::logMessage("Using %zu/%zu mb (%.2f percent)", left/MEGABYTE, total/MEGABYTE, (float)left/total*100);

    BENCHMARK_END_SESSION();

    return true;
}

void Engine::Start() {
    auto frameStart = engine_clock::now();
    auto frameEnd = frameStart + Framerate{ 1 };

    while (!done) {
        BENCHMARK_SCOPE("Frame");
        frameStart = engine_clock::now();

        {
            BENCHMARK_SCOPE("FPS Calc");
            lastFrameTime = std::chrono::duration_cast<std::chrono::microseconds>(
                frameStart - frameEnd).count();
            fpsAvg.addSample(lastFrameTime);
            dt = fpsAvg.getCurrentAverage() / 1000000.0;
        }

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

        {
            BENCHMARK_SCOPE("FPS Limiting");
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
    }

    End();
}

void Engine::Update(double dt) {
    BENCHMARK_FUNCTION();
    // Don't send input events to GameObjects while in debug mode
    gameTime += dt;
    timeAcc += dt;

    /* Every 5 seconds poll all the files watched */
    if (timeAcc > 5.0) {
        timeAcc -= 5.0;

        Console::logMessage("Checking Files");
        Post(eveCheckFileMod);
    }

    /* 4 seconds into running, start profiling */
    if (!runtimeProfile && gameTime > 4.0 && gameTime < 5.0) {
        Console::logMessage("Starting runtime benchmark");

        BENCHMARK_START_SESSION("runtime", "runtime.json");
        runtimeProfile = true;
    }

    /* 2 seconds later, stop profiling */
    if (runtimeProfile && gameTime > 6.0) {
        Console::logMessage("Ending runtime benchmark");

        BENCHMARK_END_SESSION();

        runtimeProfile = false;
    }
}

void Engine::PreRender() {
    BENCHMARK_FUNCTION();
    // Interrogate the SceneManager for a list of draw calls to make this frame.
}

void Engine::Render() {
    BENCHMARK_FUNCTION();
    windowManager->SwapAndPoll();
}

void Engine::End() {
    BENCHMARK_START_SESSION("Shutdown", "shutdown.json");

    sceneManager->Destroy();
    windowManager->Destroy();
    catalog->Destroy();
    memory->Destroy();
    fileSystem->Destroy();
    ems->Destroy();

    {
        BENCHMARK_SCOPE("wait 1.5seconds");
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }

    BENCHMARK_END_SESSION();
}