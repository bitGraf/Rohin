#include <stdio.h>

#include "CoreSystem.hpp"

#include "Window.hpp"
#include "ResourceManager.hpp"
#include "Console.hpp"

#include "MemoryManager.hpp"
#include "SceneManager.hpp"

ConfigurationManager g_ConfigManager;
MessageBus g_MessageBus;

/* Core Systems */
Window g_MainWindow;
Console g_Console;
FileSystem g_FileSystem;
ResourceManager g_ResourceManager;
SceneManager g_SceneManager;

u32 offset(void* value, void* ref) {
    u8* p1 = static_cast<u8*>(value);
    u8* p2 = static_cast<u8*>(ref);

    return (p1 - p2);
}



int main(int argc, char* argv[]) {
    EnsureDataTypeSize();

    for (int n = 0; n < 5; n++) {
        printf(".");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    printf("\n");

    // Create core systems
    g_ConfigManager.create(4,
        &g_MainWindow,
        &g_Console,
        &g_FileSystem,
        &g_ResourceManager,
        &g_SceneManager);

    g_MessageBus.create(&g_ConfigManager);

    // Other sets
    g_ResourceManager.setFileSystem(&g_FileSystem);

    /* TESTING START */
    g_ResourceManager.loadModelFromFile("Data/Models/cube.gltf");
    g_SceneManager.loadScenes(&g_ResourceManager); // Load dummy scene to test
    /*  TESTING END  */

    // Create console thread that listens for commands
    auto console_thread = std::thread(&Console::startListening, &g_Console);

    g_MainWindow.makeCurrent();

    //glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
    //glClearColor(0.716f, 0.68f, 0.652f, 1.0f);
    glClearColor(255.0f/255.0f, 248.0f/255.0f, 231.0f/255.0f, 1.0f);//Cosmic Latte, too bright :(

    g_Console.logMessage("Starting message loop.");
    bool done = false;
    while (!done) {
        if (g_MainWindow.shouldClose()) {
            done = true;
            break;
        }

        g_MessageBus.processEntireQueue();

        glClear(GL_COLOR_BUFFER_BIT);

        g_MainWindow.swapAndPoll();
    }
    g_Console.logMessage("Game loop done, quitting...");

    glfwTerminate();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Destroy all subsystems
    g_ResourceManager.destroy();
    g_FileSystem.destroy();
    g_Console.destroy();
    g_MainWindow.destroy();
    g_ConfigManager.destroy();

    // End threads
    console_thread.join();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    return 0;
}