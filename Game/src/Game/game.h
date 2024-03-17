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



#define GAME_UPDATE_FUNC(name) void name()
typedef GAME_UPDATE_FUNC(game_update);