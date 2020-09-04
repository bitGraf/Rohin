#include "Engine.hpp"

int main(int argc, char* argv[]) {
    Engine engine;
    if (!engine.Init(argc, argv)) {
        Console::logMessage("Failed to initialize Engine");

        system("pause");
        return 0;
    }

    Console::logMessage("Starting Engine loop...");
    engine.Start();
    Console::logMessage("Engine loop finished...");

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    return 0;
}