#ifndef WIN32_HANDMADE_H
#define WIN32_HANDMADE_H

#include "Game/game.h"

struct win32_offscreen_buffer {
    BITMAPINFO Info;
    void* Memory;
    int Width;
    int Height;
    int BytesPerPixel;
    int Pitch;
};

struct win32_window_dimension {
    int Width;
    int Height;
};

struct win32_game_code
{
    HMODULE GameCodeDLL;
    FILETIME DLLLastWriteTime;

    GameInit_t* Init;
    GameFrame_t* Frame;
    GameEvent_t* HandleEvent;
    GameShutdown_t* Shutdown;
    
    bool32 IsValid;
};

#define WIN32_STATE_FILE_NAME_COUNT MAX_PATH
struct win32_replay_buffer {
    HANDLE MemoryMap;
    HANDLE FileHandle;
    char ReplayFilename[WIN32_STATE_FILE_NAME_COUNT];
    void* MemoryBlock;
};

struct win32_state {
    char EXEFileName[WIN32_STATE_FILE_NAME_COUNT];
    char *OnePastLastSlash;

    game_input Input[2];
    
    WINDOWPLACEMENT WindowPosition; // save the last window position for fullscreen purposes
    int64 PerfCounterFrequency;

    uint64 TotalSize;
    void* GameMemoryBlock;
    win32_replay_buffer ReplayBuffers[2];

    HANDLE RecordingHandle;
    int InputRecordingIndex;

    HANDLE PlaybackHandle;
    int InputPlayingIndex;
};

#endif