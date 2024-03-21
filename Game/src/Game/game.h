#pragma once

#include <Engine/Defines.h>
struct render_packet;
struct scene_3D;

// .dll export api
#ifdef GAME_EXPORT
// Exports
#ifdef _MSC_VER
#define GAME_API extern "C" __declspec(dllexport)
#else
#define GAME_API extern "C" __attribute__((visibility("default")))
#endif
#else
// Imports
#define GAME_API
#endif

struct game_memory {
    bool32 IsInitialized;

    uint64 GameStorageSize;
    void*  GameStorage; // NOTE: REQUIRED to be cleared to zero at startup!!
};

#define GAME_UPDATE_FUNC(name) scene_3D* name(game_memory* memory, render_packet* packet, real32 delta_time)
typedef GAME_UPDATE_FUNC(game_update_fcn);

#define GAME_KEY_EVENT_FUNC(name) void name(game_memory* memory, uint16 key_code, uint16 key_state)
typedef GAME_KEY_EVENT_FUNC(game_key_event_fcn);