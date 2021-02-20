#pragma once

#include "Engine/Core/Base.hpp"

#ifdef RH_PLATFORM_WINDOWS
extern Engine::Application* Engine::CreateApplication();

int main(int argc, char** argv) {
    Engine::Logger::Init();

    Engine::Application* app = Engine::CreateApplication();
    app->Run();
    delete app;
}
#endif