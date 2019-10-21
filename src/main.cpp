#include <stdio.h>

#include "Window.hpp"
#include "HashTable.hpp"

#include "ResourceManager.hpp"

#include "tiny_gltf.h"
#include "optick.h"

#include "MemoryManager.hpp"

Window g_MainWindow;
Console g_Console;
MessageBus g_MessageBus;
FileSystem g_FileSystem;
ResourceManager g_ResourceManager;

int main(int argc, char* argv[]) {
    PoolAllocator pa(5, 3);
    pa.create();
    MemoryBlock* dataCopy = (MemoryBlock*)calloc(5, 3 * sizeof(MemoryBlock));
    for (int n = 0; n < 15; n++) {
        dataCopy[n] = pa.data[n];
    }
    MemoryBlock* block1 = pa.getBlock();
    block1[0] = 1;
    block1[1] = 2;
    block1[2] = 3;
    MemoryBlock* block2 = pa.getBlock();
    block2[0] = 4;
    block2[1] = 5;
    block2[2] = 6;
    pa.freeDataBlock(block2);
    pa.freeDataBlock(block1);
    pa.destroy();

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

    g_ResourceManager.create();
    g_ResourceManager.setConsole(&g_Console);
    g_ResourceManager.setMessageBus(&g_MessageBus);
    g_ResourceManager.setFileSystem(&g_FileSystem);

    g_ResourceManager.printAllResources();
    g_ResourceManager.createNewResource("res1"_sid);
    g_ResourceManager.createNewResource("res2"_sid);
    g_ResourceManager.printAllResources();
    g_ResourceManager.destroy();

    return 0;

    g_MainWindow.setConsole(&g_Console);
    g_MainWindow.setMessageBus(&g_MessageBus);
    g_MainWindow.create("Main window");

    Window window2;
    window2.setConsole(&g_Console);
    window2.setMessageBus(&g_MessageBus);
    window2.create("Other window", 400, 300);
    window2.setPosition(100, 680);

    ////////////////////
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret = loader.LoadBinaryFromFile(
        &model, &err, &warn, "Data/models/cube_bin.glb");

    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }

    if (!ret) {
        printf("Failed to parse glTF\n");
    }
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