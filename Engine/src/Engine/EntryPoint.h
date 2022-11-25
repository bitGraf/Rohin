#pragma once

#include "Engine/Core/Base.hpp"

#ifdef RH_PLATFORM_WINDOWS
extern rh::Application* rh::CreateApplication();

int main(int argc, char** argv) {
    rh::Logger::Init();

    rh::Application* app = rh::CreateApplication();
    app->Run();
    delete app;
}
#endif