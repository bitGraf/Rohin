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

struct game_memory {
    bool32 IsInitialized;

    uint64 PermanentStorageSize;
    void*  PermanentStorage; // NOTE: REQUIRED to be cleared to zero at startup!!

    uint64 TransientStorageSize;
    void*  TransientStorage; // NOTE: REQUIRED to be cleared to zero at startup!!
};
struct game_button_state {
    int HalfTransitionCount;
    bool32 EndedDown;
};

struct game_controller_input {
    bool32 IsConnected;
    bool32 IsAnalog;

    real32 StickAverageX;
    real32 StickAverageY;

    union {
        game_button_state Buttons[12];
        struct {
            game_button_state MoveUp;
            game_button_state MoveDown;
            game_button_state MoveLeft;
            game_button_state MoveRight;

            game_button_state ActionUp;
            game_button_state ActionDown;
            game_button_state ActionLeft;
            game_button_state ActionRight;

            game_button_state LeftShoulder;
            game_button_state RightShoulder;

            game_button_state Back;
            game_button_state Start;

            // NOTE: All buttons must be added above this line!
            game_button_state Terminator;
        };
    };
};

struct game_input {
    game_button_state MouseButtons[5];
    int32 MouseX, MouseY, MouseZ;

    real32 dtForFrame;

    game_controller_input Controllers[5];
};

#define GAME_UPDATE_AND_RENDER_FUNC(name) void name(game_memory* Memory, game_input* Input) //, render_command_buffer* CmdBuffer)
typedef GAME_UPDATE_AND_RENDER_FUNC(game_update_and_render);