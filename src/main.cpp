#include <stdio.h>

#include "CoreSystem.hpp"

#include "Window.hpp"
#include "ResourceManager.hpp"
#include "Console.hpp"

ConfigurationManager g_ConfigManager;
MessageBus g_MessageBus;
Window g_MainWindow;
Console g_Console;
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
    g_ConfigManager.create();

    g_MessageBus.create();
    g_MessageBus.RegisterSystem(&g_Console);
    //g_MessageBus.SetConsole(&g_Console);

    g_MainWindow.create(&g_ConfigManager);
    g_Console.create(&g_ConfigManager);
    g_FileSystem.create(&g_ConfigManager);
    g_ResourceManager.create(&g_ConfigManager);

    // Set Message Busses
    g_MainWindow.setMessageBus(&g_MessageBus);
    g_Console.setMessageBus(&g_MessageBus);
    g_FileSystem.setMessageBus(&g_MessageBus);
    g_ResourceManager.setMessageBus(&g_MessageBus);

    // Other sets
    g_ResourceManager.setFileSystem(&g_FileSystem);

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