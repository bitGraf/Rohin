/*********************************************************
 * Platform Independent code below!
 *********************************************************/

// Note: CRT library
#include "handmade.h"

/*********************************************************
 * WIN32 Platform specific code below!
 *********************************************************/
#include <windows.h>
#include <wingdi.h>
#include <stdio.h>
#include <malloc.h>
#include <xinput.h>
#include <dsound.h>
#include <gl/gl.h>

#include "win32_handmade.h"

// TODO: Global for now
global_variable bool32 GLobalRunning;
global_variable bool32 GlobalPause;
global_variable bool32 FlagCreateConsole;
global_variable win32_offscreen_buffer GLobalBackbuffer;
global_variable LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer;
global_variable int64 GlobalPerfCounterFrequency;
global_variable WINDOWPLACEMENT GlobalWindowPosition = { sizeof(GlobalWindowPosition) };
global_variable GLuint GlobalBlitTextureHandle;

// NOTE: XInputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateSub) {
    return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateSub;
#define XInputGetState XInputGetState_

// NOTE: XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateSub) {
    return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateSub;
#define XInputSetState XInputSetState_

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);

DEBUG_PLATFORM_FREE_FILE_MEMORY(DEBUGPlatformFreeFileMemory) {
    if (Memory) {
        VirtualFree(Memory, 0, MEM_RELEASE);
    }
}

DEBUG_PLATFORM_READ_ENTIRE_FILE(DEBUGPlatformReadEntireFile) {
    debug_read_file_result Result;
    Result.Contents = 0;
    Result.ContentSize = 0;

    HANDLE FileHandle = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
    if (FileHandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER FileSize;
        if (GetFileSizeEx(FileHandle, &FileSize)) {
            uint32 FileSize32 = SafeTruncateUInt64(FileSize.QuadPart);

            Result.Contents = VirtualAlloc(0, FileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (Result.Contents) {
                DWORD BytesRead = 0;
                if (ReadFile(FileHandle, Result.Contents, FileSize32, &BytesRead, 0) && (FileSize32 == BytesRead)) {
                    // NOTE: File read succesfully
                    Result.ContentSize = BytesRead;
                } else {
                    DEBUGPlatformFreeFileMemory(Thread, Result.Contents);
                    Result.Contents = 0;
                }
            }
        }

        CloseHandle(FileHandle);
    }

    return Result;
}

DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DEBUGPlatformWriteEntireFile) {
    bool32 Result = false;

    HANDLE FileHandle = CreateFileA(Filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (FileHandle != INVALID_HANDLE_VALUE) {
        DWORD BytesWritten;
        if (WriteFile(FileHandle, Memory, MemorySize, &BytesWritten, 0)) {
            // NOTE: File written succesfully
            Result = (MemorySize == BytesWritten);
        } else {
            // TODO: Logging
        }

        CloseHandle(FileHandle);
    } else {
        // TODO: Logging
    }

    return Result;
}

internal void
CatStrings(size_t SourceACount, char *SourceA,
               size_t SourceBCount, char *SourceB,
               size_t DestCount, char *Dest) {
    // TODO(casey): Dest bounds checking!
    
    for(int Index = 0; Index < SourceACount; ++Index) {
        *Dest++ = *SourceA++;
    }

    for(int Index = 0; Index < SourceBCount; ++Index) {
        *Dest++ = *SourceB++;
    }

    *Dest++ = 0;
}

internal int 
StringLength(char* String) {
    int Count = 0;
    while(*String++) {
        ++Count;
    }
    return Count;
}

internal void 
Win32BuildEXEPathFileName(win32_state* Win32State, char* FileName, int DestCount, char* Dest) {
    CatStrings(Win32State->OnePastLastSlash - Win32State->EXEFileName, Win32State->EXEFileName,
               StringLength(FileName), FileName,
               DestCount, Dest);
}

inline FILETIME
Win32GetLastWriteTime(char *Filename) {
    FILETIME LastWriteTime = {};
    
    WIN32_FILE_ATTRIBUTE_DATA Data;
    if (GetFileAttributesExA(Filename, GetFileExInfoStandard, &Data)) {
        LastWriteTime = Data.ftLastWriteTime;
    }

    return(LastWriteTime);
}

internal win32_game_code
Win32LoadGameCode(char *SourceDLLName, char *TempDLLName, char* LockFileName) {
    win32_game_code Result = {};

    WIN32_FILE_ATTRIBUTE_DATA Ignored;
    if (GetFileAttributesExA(LockFileName, GetFileExInfoStandard, &Ignored) == FALSE) {
        Result.DLLLastWriteTime = Win32GetLastWriteTime(SourceDLLName);

        CopyFileA(SourceDLLName, TempDLLName, FALSE);
    
        Result.GameCodeDLL = LoadLibraryA(TempDLLName);
        if (Result.GameCodeDLL) {
            Result.UpdateAndRender = (game_update_and_render *)
                                     GetProcAddress(Result.GameCodeDLL, "GameUpdateAndRender");
        
            Result.GetSoundSamples = (game_get_sound_samples *)
                                     GetProcAddress(Result.GameCodeDLL, "GameGetSoundSamples");

            Result.IsValid = (Result.UpdateAndRender &&
                             Result.GetSoundSamples);
        }
        else {
            OutputDebugStringA("Failed to load game code: LoadLibraryA failed!\n");
        }
    } else {
        OutputDebugStringA("Failed to load game code: lock still in place!\n");
    }

    return(Result);
}

internal void
Win32UnloadGameCode(win32_game_code *GameCode) {
    if(GameCode->GameCodeDLL) {
        FreeLibrary(GameCode->GameCodeDLL);
        GameCode->GameCodeDLL = 0;
    }

    GameCode->IsValid = false;
    GameCode->UpdateAndRender = 0;
    GameCode->GetSoundSamples = 0;
}

internal void
Win32LoadXInput() {
    HMODULE XInputLibrary;
    
    XInputLibrary = LoadLibraryA("XInput1_4.dll");
    if (XInputLibrary) {
        OutputDebugStringA("Linking to xinput1_4.dll\n");
        XInputGetState_ = (x_input_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState_ = (x_input_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
        return;
    }

    XInputLibrary = LoadLibraryA("XInput9_1_0.dll");
    if (XInputLibrary) {
        OutputDebugStringA("Linking to XInput9_1_0.dll\n");
        XInputGetState_ = (x_input_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState_ = (x_input_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
        return;
    }

    XInputLibrary = LoadLibraryA("xinput1_3.dll");
    if (XInputLibrary) {
        OutputDebugStringA("Linking to xinput1_3.dll\n");
        XInputGetState_ = (x_input_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState_ = (x_input_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
        return;
    }
}

internal void
Win32InitDSound(HWND Window, int32 BufferSize, int32 SamplesPerSecond) {
    // NOTE: load the library
    HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");

    if (DSoundLibrary) {
        direct_sound_create *DirectSoundCreate = (direct_sound_create*)GetProcAddress(DSoundLibrary, "DirectSoundCreate");

        // TODO: double check this works on Windows XP
        // NOTE: Get a DirectSound object
        LPDIRECTSOUND DirectSound;
        if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0))) {

            WAVEFORMATEX WaveFormat = {};
            WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
            WaveFormat.nChannels = 2;
            WaveFormat.nSamplesPerSec = SamplesPerSecond;
            WaveFormat.wBitsPerSample = 16;
            WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
            WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec*WaveFormat.nBlockAlign;
            WaveFormat.cbSize = 0;

            if (SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY))) {
                // NOTE: "Create" a primary buffer
                DSBUFFERDESC BufferDescription = {};
                BufferDescription.dwSize = sizeof(BufferDescription);
                BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

                LPDIRECTSOUNDBUFFER PrimaryBuffer;
                if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0))) {
                    if (SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat))) {
                        // NOTE: We have set the format!
                        OutputDebugStringA("Primary buffer format was set!\n");
                    } else {
                        //TODO: Diagnostic
                    }
                }
            } else {
                // TODO: Diagnostic
            }
            
            // NOTE: "Create" a secondary buffer
            DSBUFFERDESC BufferDescription = {};
            BufferDescription.dwSize = sizeof(BufferDescription);
            BufferDescription.dwFlags = 0;
            BufferDescription.dwBufferBytes = BufferSize;
            BufferDescription.lpwfxFormat = &WaveFormat;

            if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &GlobalSecondaryBuffer, 0))) {
                OutputDebugStringA("Secondary buffer created succesfully!\n");
            }
        } else {
            //TODO: Logging
        }
    } else {
        //TODO: Logging
    }
}

internal void
Win32InitOpenGL(HWND Window) {
    HDC WindowDC = GetDC(Window);

    PIXELFORMATDESCRIPTOR DesiredPixelFormat = {}; 
    DesiredPixelFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    DesiredPixelFormat.nVersion = 1;
    DesiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER ;
    DesiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
    DesiredPixelFormat.cColorBits = 32;
    DesiredPixelFormat.cAlphaBits = 8;
    DesiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

    int SuggestedPixelFormatIndex = ChoosePixelFormat(WindowDC, &DesiredPixelFormat);
    PIXELFORMATDESCRIPTOR SuggestedPixelFormat;
    DescribePixelFormat(WindowDC, SuggestedPixelFormatIndex, sizeof(PIXELFORMATDESCRIPTOR), &SuggestedPixelFormat);
    SetPixelFormat(WindowDC, SuggestedPixelFormatIndex, &SuggestedPixelFormat);

    HGLRC OpenGLRC = wglCreateContext(WindowDC);
    if (wglMakeCurrent(WindowDC, OpenGLRC)) {
        OutputDebugStringA("Made Current!\n");
        glGenTextures(1, &GlobalBlitTextureHandle);

        void (*glBindTextures)(GLuint,GLsizei,const GLuint*) = (void (*)(GLuint,GLsizei,const GLuint*))wglGetProcAddress("glBindTextures");
        if (glBindTextures) {
            glBindTextures(0, 0, nullptr);
            OutputDebugStringA("accessed higher version OpenGL function\n");
        }
    } else {
        Assert(!"InvalidCodePath");
    }
    ReleaseDC(Window, WindowDC);
}

internal win32_window_dimension
Win32GetWindowDimension(HWND Window) {
    win32_window_dimension Result;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;

    return Result;
}

internal void
Win32ResizeDIBSection(win32_offscreen_buffer* Buffer, int Width, int Height) {
    //TODO: Bulletproof this
    // Maybe dont free first, free after, then free first if that fails.

    if (Buffer->Memory) {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;
    Buffer->Pitch = Width * Buffer->BytesPerPixel;

    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height; // negative for a top-down DIB
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorysize = (Buffer->Width*Buffer->Height)* Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc(NULL, BitmapMemorysize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    //TODO: Clear to black?
}

internal void
Win32DisplayBufferToWindow(HDC DeviceContext,
                               int WindowWidth, int WindowHeight,
                               win32_offscreen_buffer* Buffer) {
#if 0
    if ((WindowWidth == Buffer->Width*2) && (WindowHeight == Buffer->Height*2)) {
        StretchDIBits(DeviceContext,
                      0, 0, WindowWidth, WindowHeight,
                      0, 0, Buffer->Width, Buffer->Height,
                      Buffer->Memory,
                      &Buffer->Info,
                      DIB_RGB_COLORS, SRCCOPY);
    } else {
        int OffsetX = 10;
        int OffsetY = 10;

        PatBlt(DeviceContext, 0, 0, WindowWidth, OffsetY, BLACKNESS);
        PatBlt(DeviceContext, 0, OffsetY + Buffer->Height, WindowWidth, WindowHeight, BLACKNESS);
        PatBlt(DeviceContext, 0, 0, OffsetX, WindowHeight, BLACKNESS);
        PatBlt(DeviceContext, OffsetX + Buffer->Width, 0, WindowWidth, WindowHeight, BLACKNESS);

        StretchDIBits(DeviceContext,
                      OffsetX, OffsetY, Buffer->Width, Buffer->Height,
                      0, 0, Buffer->Width, Buffer->Height,
                      Buffer->Memory,
                      &Buffer->Info,
                      DIB_RGB_COLORS, SRCCOPY);
    }
#else

    glViewport(0, 0, WindowWidth, WindowHeight);

    glBindTexture(GL_TEXTURE_2D, GlobalBlitTextureHandle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Buffer->Width, Buffer->Height, 0, 
                 GL_BGRA_EXT, GL_UNSIGNED_BYTE, Buffer->Memory);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glEnable(GL_TEXTURE_2D);

    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    real32 a = 2.0f / (real32)Buffer->Width;
    real32 b = 2.0f / (real32)Buffer->Height;
    real32 Proj[] = {
         a,  0,  0,  0,
         0,  b,  0,  0,
         0,  0,  1,  0,
        -1, -1,  0,  1
    };
    glLoadMatrixf(Proj);

    glBegin(GL_TRIANGLES);
    glTexCoord2d(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glTexCoord2d(1.0f, 0.0f);
    glVertex2f((real32)Buffer->Width, 0.0f);
    glTexCoord2d(1.0f, 1.0f);
    glVertex2f((real32)Buffer->Width, (real32)Buffer->Height);

    glTexCoord2d(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glTexCoord2d(1.0f, 1.0f);
    glVertex2f((real32)Buffer->Width, (real32)Buffer->Height);
    glTexCoord2d(0.0f, 1.0f);
    glVertex2f(0.0f, (real32)Buffer->Height);
    glEnd();

    SwapBuffers(DeviceContext);
#endif
}

internal void 
Win32ToggleFullscreen(HWND Window) {
    // TODO: Look into ChangeDisplaySettings function to change monitor refresh rate/resolution
    DWORD Style = GetWindowLong(Window, GWL_STYLE);
    if (Style & WS_OVERLAPPEDWINDOW) {
        MONITORINFO MonitorInfo = { sizeof(MonitorInfo) };
        if (GetWindowPlacement(Window, &GlobalWindowPosition) &&GetMonitorInfo(MonitorFromWindow(Window, MONITOR_DEFAULTTOPRIMARY), &MonitorInfo)) {
            SetWindowLong(Window, GWL_STYLE, Style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(Window, HWND_TOP,
                         MonitorInfo.rcMonitor.left, MonitorInfo.rcMonitor.top,
                         MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left,
                         MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } else {
        SetWindowLong(Window, GWL_STYLE,
                      Style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(Window, &GlobalWindowPosition);
        SetWindowPos(Window, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
                            UINT Message,
                            WPARAM WParam,
                            LPARAM LParam) {

    LRESULT Result = 0;

    switch (Message) {
        case WM_SIZE: {
            OutputDebugStringA("WM_SIZE\n");
        } break;

        case WM_CLOSE: {
            OutputDebugStringA("WM_CLOSE\n");

            //TODO: Handle this w/ msg to user
            GLobalRunning = false;
        } break;

        case WM_ACTIVATEAPP: {
#if 0
            if (WParam == TRUE) {
                SetLayeredWindowAttributes(Window,RGB(0, 0, 0),255,LWA_ALPHA);
            } else {
                SetLayeredWindowAttributes(Window,RGB(0, 0, 0),128,LWA_ALPHA);
            }
#endif
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_DESTROY: {
            OutputDebugStringA("WM_DESTROY\n");

            //TODO: Handle this as an error - recreate window?
            GLobalRunning = false;
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            Assert(!"Keyboard input came in through a non-dispatch method!");
        } break;

        case WM_PAINT: {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            win32_window_dimension Dimension = Win32GetWindowDimension(Window);
            Win32DisplayBufferToWindow(DeviceContext, Dimension.Width, Dimension.Height, &GLobalBackbuffer);

            EndPaint(Window, &Paint);
        } break;

        default: {
            //OutputDebugString("default\n");
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }

    return Result;
}

internal void
Win32ClearSoundBuffer(win32_sound_output *SoundOutput) {
    VOID *Region1;
    DWORD Region1Size;
    VOID *Region2;
    DWORD Region2Size;
    if (SUCCEEDED(GlobalSecondaryBuffer->Lock(0, SoundOutput->SecondaryBufferSize,
                                              &Region1, &Region1Size,
                                              &Region2, &Region2Size, 0))) {
        // TODO: Assert region1Size is valid
        uint8* DestSample = (uint8*)Region1;
        for (DWORD ByteIndex = 0; ByteIndex < Region1Size; ByteIndex++) {
            *DestSample++ = 0;
        }
        DestSample = (uint8*)Region2;
        for (DWORD ByteIndex = 0; ByteIndex < Region2Size; ByteIndex++) {
            *DestSample++ = 0;
        }

        GlobalSecondaryBuffer->Unlock(Region1, Region1Size,
                                      Region2, Region2Size);
    }
}

internal void
Win32FillSoundBuffer(win32_sound_output *SoundOutput, DWORD ByteToLock, DWORD BytesToWrite,
                         game_sound_output_buffer* SourceBuffer) {
    VOID *Region1;
    DWORD Region1Size;
    VOID *Region2;
    DWORD Region2Size;
    if (SUCCEEDED(GlobalSecondaryBuffer->Lock(ByteToLock, BytesToWrite,
                                              &Region1, &Region1Size,
                                              &Region2, &Region2Size, 0))) {

        // TODO: Assert region1Size is valid
        DWORD Region1SampleCount = Region1Size / SoundOutput->BytesPerSample;
        int16* DestSample = (int16*)Region1;
        int16* SourceSample = SourceBuffer->Samples;
        for (DWORD SampleIndex = 0; SampleIndex < Region1SampleCount; SampleIndex++) {
            *DestSample++ = *SourceSample++;
            *DestSample++ = *SourceSample++;

            ++SoundOutput->RunningSampleIndex;
        }

        DestSample = (int16*)Region2;
        DWORD Region2SampleCount = Region2Size / SoundOutput->BytesPerSample;
        for (DWORD SampleIndex = 0; SampleIndex < Region2SampleCount; SampleIndex++) {
            *DestSample++ = *SourceSample++;
            *DestSample++ = *SourceSample++;

            ++SoundOutput->RunningSampleIndex;
        }

        GlobalSecondaryBuffer->Unlock(Region1, Region1Size,
                                      Region2, Region2Size);
    }
}

internal void
Win32ProcessKeyboardMessage(game_button_state* NewState,
                                bool32 IsDown) {
    if (NewState->EndedDown != IsDown) {
        NewState->EndedDown = IsDown;
        NewState->HalfTransitionCount++;
    }
}

internal void
Win32ProcessXInputDigitalButton(DWORD XInputButtonState,
                                    game_button_state* OldState,
                                    DWORD ButtonBit,
                                    game_button_state* NewState) {

    NewState->EndedDown = (XInputButtonState & ButtonBit) == ButtonBit;
    NewState->HalfTransitionCount = (OldState->EndedDown == NewState->EndedDown) ? 1 : 0;
}

internal void 
Win32GetInputFileLocation(win32_state* Win32State, bool32 InputStream, int SlotIndex, int DestCount, char* Dest) {
    char Temp[64];
    wsprintf(Temp, "loop_edit_%d_%s.hmi", SlotIndex, InputStream ? "input" : "state");
    Win32BuildEXEPathFileName(Win32State, Temp, DestCount, Dest);
}

internal win32_replay_buffer* 
Win32GetReplayBuffer(win32_state* Win32State, int Index) {
    Assert(Index < ArrayCount(Win32State->ReplayBuffers));
    win32_replay_buffer* Result = &Win32State->ReplayBuffers[Index];
    return Result;
}

internal void
Win32BeginRecordingInput(win32_state* Win32State, int InputRecordingIndex) {
    win32_replay_buffer* ReplayBuffer = Win32GetReplayBuffer(Win32State, InputRecordingIndex);
    if (ReplayBuffer->MemoryBlock) {
        Win32State->InputRecordingIndex = InputRecordingIndex;

        char Filename[WIN32_STATE_FILE_NAME_COUNT];
        Win32GetInputFileLocation(Win32State, true, InputRecordingIndex, sizeof(Filename), Filename);
        Win32State->RecordingHandle = CreateFileA(Filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
        
#if 0
        LARGE_INTEGER FilePosition;
        FilePosition.QuadPart = Win32State->TotalSize;
        SetFilePointerEx(Win32State->RecordingHandle, FilePosition, 0, FILE_BEGIN);
#endif

        CopyMemory(ReplayBuffer->MemoryBlock, Win32State->GameMemoryBlock, Win32State->TotalSize);
    }
}

internal void
Win32EndRecordingInput(win32_state* Win32State) {
    CloseHandle(Win32State->RecordingHandle);
    Win32State->InputRecordingIndex = 0;
}

internal void
Win32BeginInputPlayback(win32_state* Win32State, int InputPlayingIndex) {
    win32_replay_buffer* ReplayBuffer = Win32GetReplayBuffer(Win32State, InputPlayingIndex);
    if (ReplayBuffer->MemoryBlock) {
        Win32State->InputPlayingIndex = InputPlayingIndex;

        char Filename[WIN32_STATE_FILE_NAME_COUNT];
        Win32GetInputFileLocation(Win32State, true, InputPlayingIndex, sizeof(Filename), Filename);
        Win32State->PlaybackHandle = CreateFileA(Filename, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);

#if 0
        LARGE_INTEGER FilePosition;
        FilePosition.QuadPart = Win32State->TotalSize;
        SetFilePointerEx(Win32State->PlaybackHandle, FilePosition, 0, FILE_BEGIN);
#endif

        CopyMemory(Win32State->GameMemoryBlock, ReplayBuffer->MemoryBlock, Win32State->TotalSize);
    }
}

internal void
Win32EndInputPlayback(win32_state* Win32State) {
    CloseHandle(Win32State->PlaybackHandle);
    Win32State->InputPlayingIndex = 0;
}

internal void 
Win32RecordInput(win32_state* Win32State, game_input* NewInput) {
    DWORD BytesWritten;
    WriteFile(Win32State->RecordingHandle, NewInput, sizeof(*NewInput), &BytesWritten, 0);
}

internal void 
Win32PlaybackInput(win32_state* Win32State, game_input* NewInput) {
    DWORD BytesRead;
    if (ReadFile(Win32State->PlaybackHandle, NewInput, sizeof(*NewInput), &BytesRead, 0)) {
        // There's still input
        if (BytesRead == 0) {
            // Looping the stream
            OutputDebugStringA("Looping input recording...\n");
            int PlayingIndex = Win32State->InputPlayingIndex; 
            Win32EndInputPlayback(Win32State);
            Win32BeginInputPlayback(Win32State, PlayingIndex);
            ReadFile(Win32State->PlaybackHandle, NewInput, sizeof(*NewInput), &BytesRead, 0);
        }
    } else {
    }
}

internal void
Win32ProcessPendingMessages(win32_state *Win32State, game_input* Input) {
    MSG Message;
    while (PeekMessageA(&Message, NULL, 0, 0, PM_REMOVE)) {
        if (Message.message == WM_QUIT)
            GLobalRunning = false;

        switch (Message.message) {
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP: {
                game_controller_input* KeyboardController = GetController(Input, 0);
                                
                uint32 VKCode = (uint32)Message.wParam;
                bool32 WasDown = ((Message.lParam & (1 << 30)) != 0);
                bool32 IsDown = ((Message.lParam & (1 << 31)) == 0);
                bool32 AltKeyWasDown = (Message.lParam & (1 << 29));

                if (WasDown != IsDown) {
                    if (VKCode == 'W') {
                        Win32ProcessKeyboardMessage(&KeyboardController->MoveUp, IsDown);
                    } else if (VKCode == 'A') {
                        Win32ProcessKeyboardMessage(&KeyboardController->MoveLeft, IsDown);
                    } else if (VKCode == 'S') {
                        Win32ProcessKeyboardMessage(&KeyboardController->MoveDown, IsDown);
                    } else if (VKCode == 'D') {
                        Win32ProcessKeyboardMessage(&KeyboardController->MoveRight, IsDown);
                    } else if (VKCode == 'Q') {
                        Win32ProcessKeyboardMessage(&KeyboardController->LeftShoulder, IsDown);
                    } else if (VKCode == 'E') {
                        Win32ProcessKeyboardMessage(&KeyboardController->RightShoulder, IsDown);
                    } else if (VKCode == VK_UP) {
                        Win32ProcessKeyboardMessage(&KeyboardController->ActionUp, IsDown);
                    } else if (VKCode == VK_LEFT) {
                        Win32ProcessKeyboardMessage(&KeyboardController->ActionLeft, IsDown);
                    } else if (VKCode == VK_DOWN) {
                        Win32ProcessKeyboardMessage(&KeyboardController->ActionDown, IsDown);
                    } else if (VKCode == VK_RIGHT) {
                        Win32ProcessKeyboardMessage(&KeyboardController->ActionRight, IsDown);
                    } else if (VKCode == VK_ESCAPE) {
                        GLobalRunning = false;
                        Win32ProcessKeyboardMessage(&KeyboardController->Start, IsDown);
                    } else if (VKCode == VK_SPACE) {
                        Win32ProcessKeyboardMessage(&KeyboardController->Back, IsDown);
                    }
#if HANDMADE_INTERNAL
                    else if (VKCode == 'P') {
                        if (IsDown)
                            GlobalPause = !GlobalPause;
                    }
                    else if(VKCode == 'L') {
                        if (IsDown) {
                            if (Win32State->InputPlayingIndex == 0) {
                                if (Win32State->InputRecordingIndex == 0) {
                                    Win32BeginRecordingInput(Win32State, 1);
                                } else {
                                    Win32EndRecordingInput(Win32State);
                                    Win32BeginInputPlayback(Win32State, 1);
                                }
                            } else {
                                Win32EndInputPlayback(Win32State);
                            }
                        }
                    }
#endif
                    else if ((VKCode == VK_F4) && AltKeyWasDown) {
                        GLobalRunning = false;
                    } else if ((VKCode == VK_RETURN) && AltKeyWasDown) {
                        if (IsDown) {
                            if (Message.hwnd) {
                                Win32ToggleFullscreen(Message.hwnd);
                            }
                        }
                    }
                }
            } break;

            case WM_LBUTTONDOWN: { 
                OutputDebugStringA("Mouse1 Down\n"); 
                Win32ProcessKeyboardMessage(&Input->MouseButtons[0], true);
            } break;
            case WM_LBUTTONUP: { 
                OutputDebugStringA("Mouse1 Up\n"); 
                Win32ProcessKeyboardMessage(&Input->MouseButtons[0], false);
            } break;

            case WM_RBUTTONDOWN: { 
                OutputDebugStringA("Mouse2 Down\n"); 
                Win32ProcessKeyboardMessage(&Input->MouseButtons[1], true);
            } break;
            case WM_RBUTTONUP: { 
                OutputDebugStringA("Mouse2 Up\n"); 
                Win32ProcessKeyboardMessage(&Input->MouseButtons[1], false);
            } break;

            case WM_MBUTTONDOWN: { 
                OutputDebugStringA("Mouse3 Down\n"); 
                Win32ProcessKeyboardMessage(&Input->MouseButtons[2], true);
            } break;
            case WM_MBUTTONUP: { 
                OutputDebugStringA("Mouse3 Up\n"); 
                Win32ProcessKeyboardMessage(&Input->MouseButtons[2], false);
            } break;

            case WM_XBUTTONDOWN: { 
                bool32 IsXButton1 = (Message.wParam & 0x00010000);
                bool32 IsXButton2 = (Message.wParam & 0x00020000);

                if (IsXButton1) {
                    OutputDebugStringA("Mouse4 Down\n");
                    Win32ProcessKeyboardMessage(&Input->MouseButtons[3], true);
                }
                else if (IsXButton2) {
                    OutputDebugStringA("Mouse5 Down\n");
                    Win32ProcessKeyboardMessage(&Input->MouseButtons[4], true);
                }
            } break;
            case WM_XBUTTONUP: { 
                bool32 IsXButton1 = (Message.wParam & 0x00010000);
                bool32 IsXButton2 = (Message.wParam & 0x00020000);

                if (IsXButton1) {
                    OutputDebugStringA("Mouse4 Up\n");
                    Win32ProcessKeyboardMessage(&Input->MouseButtons[3], false);
                }
                else if (IsXButton2) {
                    OutputDebugStringA("Mouse5 Up\n");
                    Win32ProcessKeyboardMessage(&Input->MouseButtons[4], false);
                }
            } break;

            default:
                TranslateMessage(&Message);
                DispatchMessageA(&Message);
        }
    }
}

internal real32
Win32ProcessXInputStickValue(SHORT StickValue, SHORT DeadzoneValue) {
    real32 Value = 0;

    if (StickValue < -DeadzoneValue) {
        Value = (real32)(StickValue + DeadzoneValue) / (32768.0f - DeadzoneValue);
    } else if (StickValue > DeadzoneValue) {
        Value = (real32)(StickValue - DeadzoneValue) / (32767.0f - DeadzoneValue);
    }
    return Value;
}

inline LARGE_INTEGER
Win32GetWallClock() {
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);

    return Result;
}

inline real32
Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End) {
    real32 Result = ((real32)(End.QuadPart - Start.QuadPart)) / (real32)GlobalPerfCounterFrequency;
    return Result;
}

internal void
Win32CreateConsoleAndMapStreams() {
    AllocConsole();
    
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    
    printf("stdout Mapped to console!\n");
}

internal void Win32GetEXEFileName(win32_state* Win32State) {
    // NOTE(casey): Never use MAX_PATH in code that is user-facing, because it
    // can be dangerous and lead to bad results.
    DWORD SizeOfFilename = GetModuleFileNameA(0, Win32State->EXEFileName, sizeof(Win32State->EXEFileName));
    Win32State->OnePastLastSlash = Win32State->EXEFileName;
    for(char *Scan = Win32State->EXEFileName; *Scan; ++Scan) {
        if(*Scan == '\\') {
            Win32State->OnePastLastSlash = Scan + 1;
        }
    }
}


















int CALLBACK
WinMain(HINSTANCE Instance,
            HINSTANCE PrevInstance,
            LPSTR CommandLine,
            int ShowCode) {

    win32_state Win32State = {};

    Win32GetEXEFileName(&Win32State);

    char SourceGameCodeDLLFullPath[WIN32_STATE_FILE_NAME_COUNT];
    Win32BuildEXEPathFileName(&Win32State, "handmade.dll", sizeof(SourceGameCodeDLLFullPath), SourceGameCodeDLLFullPath);

    char TempGameCodeDLLFullPath[WIN32_STATE_FILE_NAME_COUNT];
    Win32BuildEXEPathFileName(&Win32State, "handmade_temp.dll", sizeof(TempGameCodeDLLFullPath), TempGameCodeDLLFullPath);

    char GameCodeLockFullPath[WIN32_STATE_FILE_NAME_COUNT];
    Win32BuildEXEPathFileName(&Win32State, "lock.tmp", sizeof(GameCodeLockFullPath), GameCodeLockFullPath);

    LARGE_INTEGER PerfCounterFrequencyResult;
    QueryPerformanceFrequency(&PerfCounterFrequencyResult);
    GlobalPerfCounterFrequency = PerfCounterFrequencyResult.QuadPart;

    // NOTE: Set the Windows scheduler granularity to 1ms
    //       so that our Sleep can be more granular
    UINT DesiredSchedulerMS = 1;
    bool32 SleepIsGranular = (timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR);

    Win32LoadXInput();

    WNDCLASSA WindowClass = {};

    Win32ResizeDIBSection(&GLobalBackbuffer, 960, 540);

    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = LoadCursorA(NULL, IDC_CROSS);
    //WindowClass.hIcon;
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";

#if HANDMADE_INTERNAL
    // Allocate a console for this app
    if (FlagCreateConsole)
        Win32CreateConsoleAndMapStreams();
#endif

    if (RegisterClassA(&WindowClass)) {
        HWND Window = CreateWindowExA(
            0,//WS_EX_TOPMOST|WS_EX_LAYERED, 
            WindowClass.lpszClassName, "Handmade Hero",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            0, 0, Instance, 0);
        if (Window) {
            HDC DeviceContext = GetDC(Window);
            win32_sound_output SoundOutput = {};
            Win32InitOpenGL(Window);

            // How do we reliably query this on Windows?
            int MonitorRefreshHz = 60;
            int Win32RefreshRate = GetDeviceCaps(DeviceContext, VREFRESH);
            if (Win32RefreshRate > 1) {
                MonitorRefreshHz = Win32RefreshRate;
            }
            real32 GameUpdateHz = (MonitorRefreshHz / 2.0f);
            real32 TargetSecondsElapsedPerFrame = 1.0f / GameUpdateHz;

            SoundOutput.SamplesPerSecond = 48000;
            SoundOutput.RunningSampleIndex = 0;
            SoundOutput.BytesPerSample = sizeof(int16) * 2;
            SoundOutput.SecondaryBufferSize = SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample;
            // TODO: Actually compute this variance and see what the lowest reasonable value is!
            SoundOutput.SafetyBytes = (DWORD)(((real32)(SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample) / GameUpdateHz) / 3);

            Win32InitDSound(Window, SoundOutput.SecondaryBufferSize, SoundOutput.SamplesPerSecond);
            Win32ClearSoundBuffer(&SoundOutput);
            GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

            GLobalRunning = true;

#if 0
            // NOTE: This tests the PlaycursorWriteCursor update frequency
            // It was 480 samples
            while (GLobalRunning) {
                DWORD PlayCursor, WriteCursor;
                if (GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor) == DS_OK) {
                    char TextBuffer[256];
                    _snprintf_s(TextBuffer, sizeof(TextBuffer),
                                "PC:%u WC:%u\n", PlayCursor, WriteCursor);
                    OutputDebugStringA(TextBuffer);
                }
            }
#endif

            // TODO: Pool with batmap VirtualAlloc
            int16* Samples = (int16*)VirtualAlloc(NULL, SoundOutput.SecondaryBufferSize,
                                                  MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

#if HANDMADE_INTERNAL
            LPVOID BaseAddress = (LPVOID)Terabytes(2);
#else
            LPVOID BaseAddress = 0;
#endif
            game_memory GameMemory = {};
            GameMemory.PermanentStorageSize = Megabytes(64);
            GameMemory.TransientStorageSize = Gigabytes(1);
            GameMemory.DEBUGPlatformFreeFileMemory = DEBUGPlatformFreeFileMemory;
            GameMemory.DEBUGPlatformReadEntireFile = DEBUGPlatformReadEntireFile;
            GameMemory.DEBUGPlatformWriteEntireFile = DEBUGPlatformWriteEntireFile;

            Win32State.TotalSize = GameMemory.PermanentStorageSize + GameMemory.TransientStorageSize;
            // TODO: look into MEM_LARGE_PAGES
            Win32State.GameMemoryBlock = VirtualAlloc(BaseAddress, (size_t)Win32State.TotalSize,
                                                       MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            GameMemory.PermanentStorage = Win32State.GameMemoryBlock;
            GameMemory.TransientStorage = ((uint8*)GameMemory.PermanentStorage + GameMemory.PermanentStorageSize);

            for (int ReplayIndex = 0; ReplayIndex < ArrayCount(Win32State.ReplayBuffers); ReplayIndex++) {
                win32_replay_buffer* ReplayBuffer = &Win32State.ReplayBuffers[ReplayIndex];

                // TODO: These files should go to a tmp directory!
                Win32GetInputFileLocation(&Win32State, false, ReplayIndex, sizeof(ReplayBuffer->ReplayFilename), ReplayBuffer->ReplayFilename);

                ReplayBuffer->FileHandle = CreateFileA(ReplayBuffer->ReplayFilename, GENERIC_WRITE|GENERIC_READ, 0, 0, CREATE_ALWAYS, 0, 0);

                LARGE_INTEGER MaxSize;
                MaxSize.QuadPart = Win32State.TotalSize;
                ReplayBuffer->MemoryMap = CreateFileMappingA(
                                          ReplayBuffer->FileHandle, 0, 
                                          PAGE_READWRITE, 
                                          MaxSize.HighPart, MaxSize.LowPart, 0);
                DWORD mmError = GetLastError();

                ReplayBuffer->MemoryBlock = MapViewOfFile(ReplayBuffer->MemoryMap, FILE_MAP_ALL_ACCESS, 0, 0, Win32State.TotalSize);

                if(ReplayBuffer->MemoryBlock) {

                } else {
                    // TODO: Diagnostic
                }
            }

            if (Samples && GameMemory.PermanentStorage && GameMemory.TransientStorage) {

                game_input Input[2] = {};
                game_input* NewInput = &Input[0];
                game_input* OldInput = &Input[1];

                LARGE_INTEGER LastCounter = Win32GetWallClock();
                LARGE_INTEGER FlipWallClock = Win32GetWallClock();

                int DebugTimeMarkerIndex = 0;
                win32_debug_time_marker DebugTimeMarkers[30] = {};

                DWORD AudioLatencyBytes = 0;
                real32 AudioLatencySeconds = 0.0f;
                bool32 SoundIsValid = false;

                uint64 LastCycleCount = __rdtsc();

                win32_game_code Game = Win32LoadGameCode(SourceGameCodeDLLFullPath,
                                                         TempGameCodeDLLFullPath,
                                                         GameCodeLockFullPath);
                if (!Game.IsValid) {
                    MessageBoxA(Window, "Could not find/open game.dll to run!\nExiting now...", "Failed to find game.dll", MB_OK | MB_ICONERROR);
                    GLobalRunning = false;
                }

                while (GLobalRunning) {
                    NewInput->dtForFrame = TargetSecondsElapsedPerFrame;

                    FILETIME NewDLLWriteTime = Win32GetLastWriteTime(SourceGameCodeDLLFullPath);
                    if(CompareFileTime(&NewDLLWriteTime, &Game.DLLLastWriteTime) != 0) {
                        Win32UnloadGameCode(&Game);
                        Game = Win32LoadGameCode(SourceGameCodeDLLFullPath,
                                                 TempGameCodeDLLFullPath,
                                                 GameCodeLockFullPath);
                    }

                    game_controller_input *OldKeyboardController = GetController(OldInput, 0);
                    game_controller_input *NewKeyboardController = GetController(NewInput, 0);
                    // TODO: Can't zero everything because the up/down state will be wrong
                    // game_controller_input ZeroController = {};
                    *NewKeyboardController = {};
                    NewKeyboardController->IsConnected = true;
                    for (int ButtonIndex = 0; ButtonIndex < ArrayCount(NewKeyboardController->Buttons); ButtonIndex++) {
                        NewKeyboardController->Buttons[ButtonIndex].EndedDown =
                            OldKeyboardController->Buttons[ButtonIndex].EndedDown;
                    }
                    for (int ButtonIndex = 0; ButtonIndex < ArrayCount(NewInput->MouseButtons); ButtonIndex++) {
                        NewInput->MouseButtons[ButtonIndex].EndedDown =
                            OldInput->MouseButtons[ButtonIndex].EndedDown;
                    }

                    Win32ProcessPendingMessages(&Win32State, NewInput);

                    if (!GlobalPause) {
                        POINT MouseP;
                        GetCursorPos(&MouseP);
                        ScreenToClient(Window, &MouseP);
                        NewInput->MouseX = MouseP.x;
                        NewInput->MouseY = MouseP.y;
                        NewInput->MouseZ = 0;
                        //NewInput->MouseButtons[0];

                        // TODO: Need to not poll disconected controlelrs
                        // TODO: Should we poll this more frequently?
                        DWORD MaxControllerCount = XUSER_MAX_COUNT;
                        if (MaxControllerCount > (ArrayCount(NewInput->Controllers) - 1)) {
                            MaxControllerCount = (ArrayCount(NewInput->Controllers) - 1);
                        }

                        for (DWORD ControllerIndex = 0; ControllerIndex < MaxControllerCount; ControllerIndex++) {
                            DWORD OurControllerIndex = ControllerIndex + 1;
                            game_controller_input *OldController = GetController(OldInput, OurControllerIndex);
                            game_controller_input *NewController = GetController(NewInput, OurControllerIndex);

                            XINPUT_STATE ControllerState;
                            if (XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS) {
                                // this controller is plugged in!
                                XINPUT_GAMEPAD* Pad = &ControllerState.Gamepad;
                                NewController->IsConnected = true;
                                NewController->IsAnalog = OldController->IsAnalog;
                        
                                // TODO: This implements a square deadzone: maybe round would be better
                                NewController->StickAverageX = Win32ProcessXInputStickValue(Pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                                NewController->StickAverageY = Win32ProcessXInputStickValue(Pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

                                if ((NewController->StickAverageX != 0.0f) || (NewController->StickAverageY != 0.0f)) {
                                    NewController->IsAnalog = true;
                                }

                                if (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP) {
                                    NewController->StickAverageY = 1.0f;
                                    NewController->IsAnalog = false;
                                }
                                if (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
                                    NewController->StickAverageY = -1.0f;
                                    NewController->IsAnalog = false;
                                }
                                if (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
                                    NewController->StickAverageX = -1.0f;
                                    NewController->IsAnalog = false;
                                }
                                if (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
                                    NewController->StickAverageX = 1.0f;
                                    NewController->IsAnalog = false;
                                }

                                real32 ThresholdValue = 0.5f;
                                Win32ProcessXInputDigitalButton(
                                    (NewController->StickAverageY > ThresholdValue) ? 1 : 0,
                                    &OldController->MoveUp, 1,
                                    &NewController->MoveUp);
                                Win32ProcessXInputDigitalButton(
                                    (NewController->StickAverageX < -ThresholdValue) ? 1 : 0,
                                    &OldController->MoveLeft, 1,
                                    &NewController->MoveLeft);
                                Win32ProcessXInputDigitalButton(
                                    (NewController->StickAverageY < -ThresholdValue) ? 1 : 0,
                                    &OldController->MoveDown, 1,
                                    &NewController->MoveDown);
                                Win32ProcessXInputDigitalButton(
                                    (NewController->StickAverageX > ThresholdValue) ? 1 : 0,
                                    &OldController->MoveRight, 1,
                                    &NewController->MoveRight);

                                Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                                &OldController->ActionDown, XINPUT_GAMEPAD_A,
                                                                &NewController->ActionDown);
                                Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                                &OldController->ActionRight, XINPUT_GAMEPAD_B,
                                                                &NewController->ActionRight);
                                Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                                &OldController->ActionLeft, XINPUT_GAMEPAD_X,
                                                                &NewController->ActionLeft);
                                Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                                &OldController->ActionUp, XINPUT_GAMEPAD_Y,
                                                                &NewController->ActionUp);
                                Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                                &OldController->LeftShoulder, XINPUT_GAMEPAD_LEFT_SHOULDER,
                                                                &NewController->LeftShoulder);
                                Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                                &OldController->RightShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER,
                                                                &NewController->RightShoulder);

                                Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                                &OldController->Start, XINPUT_GAMEPAD_START,
                                                                &NewController->Start);
                                Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                                &OldController->Back, XINPUT_GAMEPAD_BACK,
                                                                &NewController->Back);

                            } else {
                                // This controller is not available!
                                NewController->IsConnected = false;
                            }
                        }

                        //XINPUT_VIBRATION Vibration;
                        //Vibration.wLeftMotorSpeed  = 20000;
                        //Vibration.wRightMotorSpeed = 20000;
                        //XInputSetState(0, &Vibration);

                        thread_context ThreadContext = {};

                        game_offscreen_buffer Buffer;
                        Buffer.Memory = GLobalBackbuffer.Memory;
                        Buffer.Width = GLobalBackbuffer.Width;
                        Buffer.Height = GLobalBackbuffer.Height;
                        Buffer.Pitch = GLobalBackbuffer.Pitch;
                        Buffer.BytesPerPixel = GLobalBackbuffer.BytesPerPixel;
    
                        if (Win32State.InputRecordingIndex) {
                            Win32RecordInput(&Win32State, NewInput);
                        }

                        if (Win32State.InputPlayingIndex) {
                            Win32PlaybackInput(&Win32State, NewInput);
                        }
                        if (Game.UpdateAndRender) {
                            Game.UpdateAndRender(&ThreadContext, &GameMemory, NewInput, &Buffer);
                        }

                        LARGE_INTEGER AudioWallClock = Win32GetWallClock();
                        real32 FromBeginToAudioSeconds = Win32GetSecondsElapsed(FlipWallClock, AudioWallClock);
    
                        DWORD PlayCursor;
                        DWORD WriteCursor;
                        if (GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor) == DS_OK) {
                            /* NOTE: 
                            * 
                            * Here is how sound output computation works.
                            * 
                            * When we wake up to write audio, we will look 
                            * and see what the play cursor position is and we 
                            * will forecast ahead where we think the play 
                            * cursor will be on the next frame boundary
                            * 
                            * We will then look to see if the write cursor is 
                            * before that. If it is, the target fill 
                            * position is that frame boundary plus one frame.
                            * This gives us perfect audio sync in the case of
                            * a card that has low enough latency.
                            * 
                            * If the write cursor is _after_ the next frame 
                            * boundary, then we assume we can never sync the 
                            * audio perfectly, so we will write one frame's
                            * worth of audio plus som number of guard samples
                            * (1ms, or something determined to be 
                            * safe, whatever we this the variability of our 
                            * frame computation is).
                            * 
                            * */
    
                            if (!SoundIsValid) {
                                SoundOutput.RunningSampleIndex = WriteCursor / SoundOutput.BytesPerSample;
                                SoundIsValid = true;
                            }
    
                            DWORD ByteToLock = (SoundOutput.RunningSampleIndex*SoundOutput.BytesPerSample) %
                                               SoundOutput.SecondaryBufferSize;
    
                            DWORD ExpectedSoundBytesPerFrame = (DWORD)((real32)(SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample) / GameUpdateHz);
                            real32 SecondsLeftUntilFlip = (TargetSecondsElapsedPerFrame - FromBeginToAudioSeconds);
                            DWORD ExpectedBytesUntilFlip = (DWORD)((SecondsLeftUntilFlip/TargetSecondsElapsedPerFrame)*(real32)ExpectedSoundBytesPerFrame);
    
                            DWORD ExpectedFrameBoundaryByte = PlayCursor + ExpectedBytesUntilFlip;
    
                            DWORD SafeWriteCursor = WriteCursor;
                            if (SafeWriteCursor < PlayCursor) {
                                SafeWriteCursor += SoundOutput.SecondaryBufferSize;
                            }
                            Assert(SafeWriteCursor >= PlayCursor);
                            SafeWriteCursor += SoundOutput.SafetyBytes;

                            bool32 AudioCardIsLowLatency = (SafeWriteCursor < ExpectedFrameBoundaryByte);
    
                            DWORD TargetCursor = 0;
                            if (AudioCardIsLowLatency) {
                                TargetCursor = (ExpectedFrameBoundaryByte + ExpectedSoundBytesPerFrame);
                            } else {
                                TargetCursor =
                                    (WriteCursor + ExpectedSoundBytesPerFrame + SoundOutput.SafetyBytes);
                            }
                            TargetCursor = (TargetCursor % SoundOutput.SecondaryBufferSize);
    
                            DWORD BytesToWrite = 0;
                            if (ByteToLock > TargetCursor) {
                                //BytesToWrite = SoundOutput.SecondaryBufferSize - (ByteToLock - TargetCursor);
                                BytesToWrite = (SoundOutput.SecondaryBufferSize - ByteToLock);
                                BytesToWrite += TargetCursor;
                            } else {
                                BytesToWrite = TargetCursor - ByteToLock;
                            }
    
                            // TODO: Compute how much sound to write and where
                            game_sound_output_buffer SoundBuffer = {};
                            SoundBuffer.SamplesPerSecond = SoundOutput.SamplesPerSecond;
                            SoundBuffer.SampleCount = BytesToWrite / SoundOutput.BytesPerSample;
                            SoundBuffer.Samples = Samples;
                            if (Game.GetSoundSamples) {
                                Game.GetSoundSamples(&ThreadContext, &GameMemory, &SoundBuffer);
                            }
    
#if HANDMADE_INTERNAL
                            win32_debug_time_marker* Marker = &DebugTimeMarkers[DebugTimeMarkerIndex];

                            Marker->OutputPlayCursor  = PlayCursor;
                            Marker->OutputWriteCursor = WriteCursor;
                            Marker->OutputLocation    = ByteToLock;
                            Marker->OutputByteCount   = BytesToWrite;
                            Marker->ExpectedFlipPlayCursor = ExpectedFrameBoundaryByte;
    
                            DWORD UnwrappedWriteCursor = WriteCursor;
                            if (UnwrappedWriteCursor < PlayCursor) {
                                UnwrappedWriteCursor += SoundOutput.SecondaryBufferSize;
                            }
                            AudioLatencyBytes = UnwrappedWriteCursor - PlayCursor;
                            AudioLatencySeconds = (((real32)AudioLatencyBytes / (real32)SoundOutput.BytesPerSample) /
                                                  (real32)SoundOutput.SamplesPerSecond);
    
                            char TextBuffer[256];
                            _snprintf_s(TextBuffer, sizeof(TextBuffer),
                                        "BTL:%u, TC:%u, BTW:%u - PC:%u WC:%u DELTA:%u (%fs)\n",
                                        ByteToLock, TargetCursor, BytesToWrite,
                                        PlayCursor, WriteCursor, AudioLatencyBytes, AudioLatencySeconds);
                            //OutputDebugStringA(TextBuffer);
#endif
    
                            Win32FillSoundBuffer(&SoundOutput, ByteToLock, BytesToWrite, &SoundBuffer);
                        } else {
                            SoundIsValid = false;
                        }
    
                        LARGE_INTEGER WorkCounter = Win32GetWallClock();
                        real32 WorkSecondsElapsed = Win32GetSecondsElapsed(LastCounter, WorkCounter);
    
                        // TODO: Untested! buggy
                        real32 SecondsElapsedForFrame = WorkSecondsElapsed;
                        if (SecondsElapsedForFrame < TargetSecondsElapsedPerFrame) {
                            if (SleepIsGranular) {
                                DWORD SleepMS = (DWORD)(1000.0f*(TargetSecondsElapsedPerFrame - SecondsElapsedForFrame));
                                if (SleepMS > 0) {
                                    Sleep(SleepMS);
                                }
                            }
    
                            while (SecondsElapsedForFrame < TargetSecondsElapsedPerFrame) {
                                SecondsElapsedForFrame = Win32GetSecondsElapsed(LastCounter, Win32GetWallClock());
                            }
                        } else {
                            // TODO: Missed Frame Rate!
                            // TODO: Logging
                        }
    
                        LARGE_INTEGER EndCounter = Win32GetWallClock();
                        real32 MSPerFrame = 1000.0f * Win32GetSecondsElapsed(LastCounter, EndCounter);
                        LastCounter = EndCounter;

#if 0
#if HANDMADE_INTERNAL
                        Win32DebugSyncDisplay(&GLobalBackbuffer, ArrayCount(DebugTimeMarkers), DebugTimeMarkers, DebugTimeMarkerIndex-1, &SoundOutput, TargetSecondsElapsedPerFrame);
#endif
#endif
    
                        win32_window_dimension Dimension = Win32GetWindowDimension(Window);
                        Win32DisplayBufferToWindow(DeviceContext, Dimension.Width, Dimension.Height, &GLobalBackbuffer);
    
                        FlipWallClock = Win32GetWallClock();
#if HANDMADE_INTERNAL
                        // NOTE: Debug code
                        {
                            DWORD _PlayCursor;
                            DWORD _WriteCursor;
                            if (GlobalSecondaryBuffer->GetCurrentPosition(&_PlayCursor, &_WriteCursor) == DS_OK) {
                                Assert(DebugTimeMarkerIndex < ArrayCount(DebugTimeMarkers));
                                win32_debug_time_marker* Marker = &DebugTimeMarkers[DebugTimeMarkerIndex];
    
                                Marker->FlipPlayCursor = _PlayCursor;
                                Marker->FlipWriteCursor = _WriteCursor;
                            }
                        }
#endif
    
                        game_input* Temp = NewInput;
                        NewInput = OldInput;
                        OldInput = Temp;
    
#if 0
                        uint64 EndCycleCount = __rdtsc();
                        uint64 CyclesElapsed = EndCycleCount - LastCycleCount;
                        LastCycleCount = EndCycleCount;
    
                        real32 FPS = 1000.0f / MSPerFrame;
                        real32 MCPF = ((real32)CyclesElapsed / (1000.0f*1000.0f));
    
                        char FPSBuffer[256];
                        snprintf(FPSBuffer, sizeof(FPSBuffer), "Frame: %.02f ms  %.02ffps   %.02f MHz\n", MSPerFrame, FPS, MCPF);
                        OutputDebugStringA(FPSBuffer);
#endif
    
#if HANDMADE_INTERNAL
                        DebugTimeMarkerIndex++;
                        if (DebugTimeMarkerIndex >= ArrayCount(DebugTimeMarkers)) {
                            DebugTimeMarkerIndex = 0;
                        }
#endif
                    }
                }
            } else {
                //TODO: Logging
            }
        } else {
            //TODO: Logging
        }
    } else {
        //TODO: Logging
    }

    return 0;
}