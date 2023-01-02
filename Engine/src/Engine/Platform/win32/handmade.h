#ifndef HANDMADE_H
#define HANDMADE_H

#include <math.h>
#include <stdint.h>

#define local_persist   static
#define global_variable static
#define internal        static

#define Pi32 3.14159265359f

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int32 bool32;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float  real32;
typedef double real64;

typedef size_t memory_index;

/*
 * NOTE: HANDMADE_INTERNAL:
 *       0 - Build for public release
 *       1 - Build for developer only
 * 
 * NOTE: HANDMADE_SLOW:
 *       0 - No slow code allowed!
 *       1 - Slow code welcome
 * */

#if HANDMADE_SLOW
#define Assert(Expression) if (!(Expression)) { *(int *)0 = 0; }
#else
//#define Assert(Expression)
#endif

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

inline uint32
SafeTruncateUInt64(uint64 Value) {
    // TODO: Defines for maximum values: EX: UInt32Max
    Assert(Value <= 0xFFFFFFFF);
    uint32 Result = (uint32)Value;
    return Result;
}

struct thread_context {
    int Placeholder;
};

/*
 * NOTE: Services that the platform layer provides to the game:
 */
#if HANDMADE_INTERNAL
    struct debug_read_file_result {
        uint32 ContentSize;
        void* Contents;
    };

    #define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void  name(thread_context* Thread, void* Memory)
    typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

    #define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(thread_context* Thread, char* Filename)
    typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

    #define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) bool32 name(thread_context* Thread, char* Filename, uint32 MemorySize, void* Memory)
    typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);
#endif

/*
 * NOTE: Services that the game provides to the platform layer:
 */

// FOUR THINGS - timing, Controller/keyboard input, bitmap to use, sound buffer to use

struct game_offscreen_buffer {
    /*
    *          Pixel + 00 01 02 03
    * Pixel in memory: BB GG RR xx
    * LITTLE ENDIAN ARCHITECTURE
    * 0x xxRRGGBB
    */
    void* Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

struct game_sound_output_buffer {
    int16* Samples;
    int SamplesPerSecond;
    int SampleCount;
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
inline game_controller_input* GetController(game_input* Input, int ControllerIndex) {
    Assert(ControllerIndex < ArrayCount(Input->Controllers));

    game_controller_input* Controller = &Input->Controllers[ControllerIndex];
    return Controller;
}

struct game_memory {
    bool32 IsInitialized;

    uint64 PermanentStorageSize;
    void*  PermanentStorage; // NOTE: REQUIRED to be cleared to zero at startup!!

    uint64 TransientStorageSize;
    void*  TransientStorage; // NOTE: REQUIRED to be cleared to zero at startup!!

    debug_platform_free_file_memory* DEBUGPlatformFreeFileMemory;
    debug_platform_read_entire_file* DEBUGPlatformReadEntireFile;
    debug_platform_write_entire_file* DEBUGPlatformWriteEntireFile;
};

#define GAME_UPDATE_AND_RENDER(name) void name(thread_context* Thread, game_memory* Memory, game_input* Input, game_offscreen_buffer* Buffer)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

// NOTO: At the moment, this needs to be a very fast function! It cannot be more than a millisecond or so.
// TODO: Reduce pressure on this func's performance by measuring it
#define GAME_GET_SOUND_SAMPES(name) void name(thread_context* Thread, game_memory* Memory, game_sound_output_buffer* SoundBuffer)
typedef GAME_GET_SOUND_SAMPES(game_get_sound_samples);


//
//
//
//

struct memory_arena {
    memory_index Size;
    uint8* Base;
    memory_index Used;
};

struct tile_map {
    int sizex, sizey;
    void* data;
};

struct world {
    tile_map *TileMap;
};

struct game_state {
    memory_arena WorldArena;

    world* World;
};

#endif