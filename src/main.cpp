#include <stdio.h>

#include "Window.hpp"
#include "HashTable.hpp"

#include "FileSystem.hpp"

#include "optick.h"

Window g_MainWindow;
Console g_Console;
MessageBus g_MessageBus;
FileSystem g_FileSystem;

int main(int argc, char* argv[]) {
    EnsureDataTypeSize();

    for (int n = 0; n < 5; n++) {
        printf(".");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    printf("\n");

    // Create subsystems
    g_MessageBus.create();

    g_Console.create(&g_MessageBus);

    g_FileSystem.create();
    g_FileSystem.setMessageBus(&g_MessageBus);
    g_FileSystem.setConsole(&g_Console);

    g_MainWindow.setConsole(&g_Console);
    g_MainWindow.setMessageBus(&g_MessageBus);
    g_MainWindow.create("Main window");

    Window window2;
    window2.setConsole(&g_Console);
    window2.setMessageBus(&g_MessageBus);
    window2.create("Other window", 400, 300);
    window2.setPosition(100, 680);

    ////////////////////
    u8 buffer[4096];
    size_t dataRead = 0;
    g_FileSystem.syncReadFile("Data/models/cube_bin.glb", buffer, 4096, dataRead);
    printf("Buffer read: [%d] \n\n'%s'\n\n", dataRead, buffer);
    ////////////////////

    // Create console thread that listens for commands
    auto console_thread = std::thread(&Console::startListening, &g_Console);

    g_MainWindow.makeCurrent();

    glClearColor(0.2f, 0.5f, 0.3f, 1.0f);

    g_Console.logMessage("Starting message loop.");
    bool done = false;
    while (!done) {
        OPTICK_FRAME("MainThread"); // Profiling

        if (g_MainWindow.shouldClose()) {
            done = true;
            break;
        }

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