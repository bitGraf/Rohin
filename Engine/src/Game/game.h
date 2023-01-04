#ifndef GAME_H
#define GAME_H

#include "Engine/Core/Base.hpp"

#if ROHIN_SLOW
#define Assert(Expression) if (!(Expression)) { *(int *)0 = 0; }
#else
#define Assert(Expression)
#endif

struct thread_context {
    int Placeholder;
};

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
inline game_controller_input* 
    GetController(game_input* Input, int ControllerIndex) {
    Assert(ControllerIndex < ArrayCount(Input->Controllers));

    game_controller_input* Controller = &Input->Controllers[ControllerIndex];
    return Controller;
}

// Functions that the Engine.exe calls from Game.dll
#define GAME_INIT_FUNC(init_name) void init_name(game_memory* Memory)
typedef GAME_INIT_FUNC(GameInit_t);

#define GAME_FRAME_FUNC(frame_name) void frame_name(game_memory* Memory, game_input* Input)
typedef GAME_FRAME_FUNC(GameFrame_t);

#define GAME_SHUTDOWN_FUNC(shutdown_name) void shutdown_name(game_memory* Memory)
typedef GAME_SHUTDOWN_FUNC(GameShutdown_t);

#define GAME_EVENT_FUNC(handle_event_name) void handle_event_name(game_memory* Memory)
typedef GAME_EVENT_FUNC(GameEvent_t);

struct gameExport_t {
    int Version;
    
    GameInit_t* Init;
    GameFrame_t* Frame;
    GameEvent_t* HandleEvent;
    GameShutdown_t* Shutdown;
};

// Services that the Engine provides to the Game

#define ENGINE_GET_EXE_PATH(name) const char* name(void)
typedef ENGINE_GET_EXE_PATH(EngineGetEXEPath_t);

struct gameImport_t {
    int Version;
    
    EngineGetEXEPath_t* GetEXEPath;
};

#define GAME_GET_API_FUNC(name) gameExport_t name(gameImport_t* Import)
typedef GAME_GET_API_FUNC(GameGetApi_t);


#endif