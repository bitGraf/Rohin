#if 0

#pragma once

#include <Engine/defines.h>
#include <Game/Game.h>

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <timeapi.h>
#include <strsafe.h>
#include <xinput.h>

struct win32_window_dimension {
    int Width;
    int Height;
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
    char *OnePastLastSlashEXEFileName;

    char ResourcePathPrefix[WIN32_STATE_FILE_NAME_COUNT];
    uint32 ResourcePrefixLength;
    
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

struct win32_game_code
{
    HMODULE GameCodeDLL;
    FILETIME DLLLastWriteTime;

    game_update_and_render* GameUpdateAndRender;
    
    bool32 IsValid;
};


// NOTE: XInputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(x_input_get_state);

// NOTE: XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);

#endif