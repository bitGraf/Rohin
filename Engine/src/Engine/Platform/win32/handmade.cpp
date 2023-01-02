#include "handmade.h"

internal void
GameOutputSound(game_state* GameState, game_sound_output_buffer* SoundBuffer, int ToneHz) {
    int16 ToneVolume = 1500;
    int WavePeriod = SoundBuffer->SamplesPerSecond / ToneHz;

    int16* SampleOut = SoundBuffer->Samples;
    for (int SampleIndex = 0; SampleIndex < SoundBuffer->SampleCount; ++SampleIndex) {
#if 0
        real32 SineValue = sinf(GameState->tSine);
        int16 SampleValue = (int16)(SineValue * ToneVolume);
#else
        int16 SampleValue = 0;
#endif
        *SampleOut++ = SampleValue;
        *SampleOut++ = SampleValue;

#if 0
        GameState->tSine += 2.0f*Pi32*1.0f / (real32)WavePeriod;
        if (GameState->tSine > 2.0f*Pi32) {
            GameState->tSine -= 2.0f*Pi32;
        }
#endif
    }
}

internal int32 
RoundReal32ToInt32(real32 RealValue) {
    int32 Result = (int32)(RealValue + 0.5f);
    return Result;
}

internal uint32 
    RoundReal32ToUInt32(real32 RealValue) {
    uint32 Result = (uint32)(RealValue + 0.5f);
    return Result;
}

internal void
DrawRectangle(game_offscreen_buffer* BackBuffer, 
              real32 RealMinX, real32 RealMinY,
              real32 RealMaxX, real32 RealMaxY,
              real32 R, real32 G, real32 B) {
    
    int32 MinX = RoundReal32ToInt32(RealMinX);
    int32 MinY = RoundReal32ToInt32(RealMinY);
    int32 MaxX = RoundReal32ToInt32(RealMaxX);
    int32 MaxY = RoundReal32ToInt32(RealMaxY);

    if (MinX < 0) {
        MinX = 0;
    }
    if (MinY < 0) {
        MinY = 0;
    }
    if (MaxX > BackBuffer->Width) {
        MaxX = BackBuffer->Width;
    }
    if (MaxY > BackBuffer->Height) {
        MaxY = BackBuffer->Height;
    }

    // BIT PATTERN: 0x xx RR GG BB
    uint32 Color = (uint32)(
                   (RoundReal32ToUInt32(R*255.0f) << 16) |
                   (RoundReal32ToUInt32(G*255.0f) << 8) |
                   (RoundReal32ToUInt32(B*255.0f) << 0));

    uint8* Row = ((uint8*)BackBuffer->Memory +
                   MinX*BackBuffer->BytesPerPixel +
                   MinY*BackBuffer->Pitch);
    for (int32 Y = MinY; Y < MaxY; Y++) {
        uint32* Pixel = (uint32*)Row;
        for (int32 X = MinX; X < MaxX; X++) {
            *Pixel++ = Color;
        }
        Row += BackBuffer->Pitch;
    }
}

internal void InitializeArena(memory_arena* Arena, memory_index Size, uint8* Base) {
    Arena->Size = Size;
    Arena->Base = Base;
    Arena->Used = 0;
}

#define PushStruct(Arena, type) (type*)PushStruct_(Arena, sizeof(type))
internal void* PushStruct_(memory_arena* Arena, memory_index Size) {
    Assert((Arena->Used + Size) <= Arena->Size);
    void* Result = Arena->Base + Arena->Used;
    Arena->Used += Size;

    return Result;
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender) {
    Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) == 
           (ArrayCount(Input->Controllers[0].Buttons)));
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);

    game_state* GameState = (game_state*)Memory->PermanentStorage;
    if (!Memory->IsInitialized) {

        InitializeArena(&GameState->WorldArena, 
                        Memory->PermanentStorageSize - sizeof(game_state), 
                        (uint8*)Memory->PermanentStorage + sizeof(game_state));

        GameState->World = PushStruct(&GameState->WorldArena, world);

        // TODO: This may be better in the platform layer
        Memory->IsInitialized = true;
    }

    for (int ControllerIndex = 0; ControllerIndex < ArrayCount(Input->Controllers); ControllerIndex++) {

        game_controller_input* Controller = GetController(Input, ControllerIndex);
    
        if (Controller->IsAnalog) {
            // analog movement tuning
        } else {
            // digital movement tuning
        }
    }

    uint32 TileMap[9][16] = {
        {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
        {0, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
        {0, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
        {0, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
        {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
        {0, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
        {0, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
        {0, 1, 1, 1,  1, 1, 1, 1,  1, 1, 0, 0,  0, 0, 0, 0},
        {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
    };

    real32 UpperLeftX = 10.0f;
    DrawRectangle(Buffer, 0.0f, 0.0f, (real32)Buffer->Width, (real32)Buffer->Height, 1.0f, 0.0f, 1.0f);

    real32 TileWidth = (real32)Buffer->Width/16.0f;
    real32 TileHeight = (real32)Buffer->Height/9.0f;
    for (int Row = 0; Row < 9; Row++) {
        for (int Column = 0; Column < 16; Column++) {
            uint32 TileID = TileMap[Row][Column];
            real32 Grey = 0.5f;
            if (TileID == 1) {
                Grey = 1.0f;
            }

            real32 TileX = Column*TileWidth;
            real32 TileY = Row*TileHeight;

            DrawRectangle(Buffer, TileX, TileY, TileX+TileWidth, TileY+TileHeight, Grey, Grey, Grey);
        }
    }
}

extern "C" GAME_GET_SOUND_SAMPES(GameGetSoundSamples) {
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);

    game_state* GameState = (game_state*)Memory->PermanentStorage;

    GameOutputSound(GameState, SoundBuffer, 400);
}

#if 0
internal void
RenderWeirdGradient(game_offscreen_buffer* Buffer, int BlueOffset, int GreenOffset) {
    uint8* Row = (uint8*)Buffer->Memory;
    for (int y = 0; y < Buffer->Height; y++) {
        uint32* Pixel = (uint32*)Row;
        for (int x = 0; x < Buffer->Width; x++) {
            /*
             *          Pixel + 00 01 02 03
             * Pixel in memory: BB GG RR xx
             * LITTLE ENDIAN ARCHITECTURE
             * 0x xxRRGGBB
             */

            uint8 Blue = (uint8)(x + BlueOffset);
            uint8 Green = (uint8)(y + GreenOffset);
            uint8 Red = 0;

            *Pixel = (Red << 16) | (Green << 8) | Blue;
            Pixel++;
        }

        Row += Buffer->Pitch;
    }
}
#endif