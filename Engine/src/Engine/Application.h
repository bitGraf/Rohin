#pragma once

#include "Engine/Defines.h"

struct render_packet;

struct RohinAppConfig {
    const char* application_name;
    int32 start_x;
    int32 start_y;
    int32 start_width;
    int32 start_height;

    uint64 requested_permanant_memory;
    uint64 requested_transient_memory;
};

struct GameMemory {
    bool32 IsInitialized;

    uint64 PermanentStorageSize;
    void*  PermanentStorage; // NOTE: REQUIRED to be cleared to zero at startup!!

    uint64 TransientStorageSize;
    void*  TransientStorage; // NOTE: REQUIRED to be cleared to zero at startup!!
};

struct RohinApp {
    RohinAppConfig app_config;

    // function pointers to game code
    bool32 (*startup)(RohinApp* app);
    bool32 (*initialize)(RohinApp* app);
    bool32 (*update_and_render)(RohinApp* app, render_packet* packet, real32 delta_time);
    void (*on_resize)(RohinApp* app, uint32 new_width, uint32 new_height);
    void (*shutdown)(RohinApp* app);

    GameMemory memory;
};