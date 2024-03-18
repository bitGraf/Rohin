#pragma once

#include <Engine/Defines.h>

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

struct game_state {
    uint32 value;
};

#define GAME_UPDATE_FUNC(name) void name(game_state* state)
typedef GAME_UPDATE_FUNC(game_update_fcn);