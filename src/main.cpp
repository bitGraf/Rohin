#include <stdio.h>

#include "System/CoreSystem.hpp"

#include "System/Window.hpp"
#include "System/ResourceManager.hpp"
#include "System/Console.hpp"

#include "MemoryManager.hpp"
#include "System/SceneManager.hpp"
#include "System/RenderManager.hpp"

/* Core Systems */
Window g_MainWindow;
Console g_Console;
FileSystem g_FileSystem;
ResourceManager g_ResourceManager;
SceneManager g_SceneManager;
RenderManager g_RenderManager;


int main(int argc, char* argv[]) {
    EnsureDataTypeSize();

    for (int n = 0; n < 5; n++) {
        printf(".");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    printf("\n");

    // Create core systems
    Configuration::create();
    MessageBus::create();

    MessageBus::registerSystem(g_MainWindow.create());
    MessageBus::registerSystem(g_Console.create());
    MessageBus::registerSystem(g_FileSystem.create());
    MessageBus::registerSystem(g_ResourceManager.create());
    MessageBus::registerSystem(g_SceneManager.create());
    MessageBus::registerSystem(g_RenderManager.create());

    Configuration::listMessageTypes();

    // Other sets
    g_ResourceManager.setFileSystem(&g_FileSystem);

    /* TESTING START */
    //g_ResourceManager.loadModelFromFile("Data/Models/cube.gltf", false);
    g_ResourceManager.loadModelFromFile("Data/Models/Corset.glb", true);
    g_SceneManager.loadScenes(&g_ResourceManager); // Load dummy scene to test
    g_ResourceManager.createGrid(.5, 41, 10);
    /*  TESTING END  */

    // Create console thread that listens for commands
    auto console_thread = std::thread(&Console::startListening, &g_Console);

    g_MainWindow.makeCurrent();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(10);
    //glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
    //glClearColor(0.716f, 0.68f, 0.652f, 1.0f);
    //glClearColor(255.0f/255.0f, 248.0f/255.0f, 231.0f/255.0f, 1.0f);//Cosmic Latte, too bright :(
    glClearColor(0, 0, 0, 1);

    g_Console.logMessage("Starting message loop.");
    bool done = false;
    while (!done) {
        if (g_MainWindow.shouldClose()) {
            done = true;
            break;
        }

        MessageBus::processEntireQueue();

        g_SceneManager.update(.005);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        g_RenderManager.renderScene(g_SceneManager.getCurrentScene());

        g_MainWindow.swapAndPoll();
    }
    g_Console.logMessage("Game loop done, quitting...");

    glfwTerminate();

    //std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Destroy all subsystems
    g_ResourceManager.destroy();
    g_FileSystem.destroy();
    g_Console.destroy();
    g_MainWindow.destroy();
    //g_ConfigManager.destroy();

    // End threads
    console_thread.join();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    return 0;
}