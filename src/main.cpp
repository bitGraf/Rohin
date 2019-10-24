#include <stdio.h>

#include "Window.hpp"
#include "HashTable.hpp"

#include "ResourceManager.hpp"

#include "tiny_gltf.h"
#include "optick.h"

Window g_MainWindow;
Console g_Console;
MessageBus g_MessageBus;
FileSystem g_FileSystem;
ResourceManager g_ResourceManager;

int main(int argc, char* argv[]) {
    EnsureDataTypeSize();

    for (int n = 0; n < 5; n++) {
        printf(".");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    printf("\n");

    // Create subsystems
    g_MessageBus.create();
    g_MessageBus.SetConsole(&g_Console);

    g_Console.create(&g_MessageBus);

    g_FileSystem.create();
    g_FileSystem.setMessageBus(&g_MessageBus);
    g_FileSystem.setConsole(&g_Console);

    g_ResourceManager.create();
    g_ResourceManager.setConsole(&g_Console);
    g_ResourceManager.setMessageBus(&g_MessageBus);
    g_ResourceManager.setFileSystem(&g_FileSystem);

    g_ResourceManager.loadModelFromFile("Data/Models/cube_emb.gltf");
    //return 0;

    g_ResourceManager.printAllResources();
    g_ResourceManager.createNewResource("res1"_sid);
    g_ResourceManager.createNewResource("res2"_sid);
    g_ResourceManager.printAllResources();
    g_ResourceManager.destroy();

    //return 0;

    g_MainWindow.setConsole(&g_Console);
    g_MainWindow.setMessageBus(&g_MessageBus);
    g_MainWindow.create("Main window");

    Window window2;
    window2.setConsole(&g_Console);
    window2.setMessageBus(&g_MessageBus);
    window2.create("Other window", 400, 300);
    window2.setPosition(100, 680);

    ////////////////////
    ////////////////////

    // Create console thread that listens for commands
    auto console_thread = std::thread(&Console::startListening, &g_Console);

    g_MainWindow.makeCurrent();

    //glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
    //glClearColor(0.716f, 0.68f, 0.652f, 1.0f);
    glClearColor(255.0f/255.0f, 248.0f/255.0f, 231.0f/255.0f, 1.0f);//Cosmic Latte, too bright :(

    g_Console.logMessage("Starting message loop.");
    bool done = false;
    while (!done) {
        OPTICK_FRAME("MainThread"); // Profiling

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

    // Rejoin threads together
    g_Console.killConsole();
    console_thread.join();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    return 0;
}