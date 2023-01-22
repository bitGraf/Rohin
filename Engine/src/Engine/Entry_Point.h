#pragma once

#include "Engine/Core/Logger.h"
#include "Engine/Application.h"
#include "Engine/Core/Engine.h"

// todo: remove this?
#include "Engine/Platform/Platform.h"

extern bool32 create_application(RohinApp* app);

int main() {
    RohinApp app;

    if (!create_application(&app)) {
        RH_FATAL("Could not create application!");
        return -1;
    }

    if (!app.update_and_render || !app.initialize || !app.on_resize) {
        RH_FATAL("Application function pointers not setup!");
        return -2;
    }

    // begin the game-loop
    if (!start_rohin_engine(&app)) {
        RH_FATAL("Something went wrong! shutting down...");
        platform_sleep(2500);
        return -3;
    }

    return 0;
}