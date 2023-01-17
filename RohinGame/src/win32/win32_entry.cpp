#if 0

#include "win32_entry.h"

#include <Engine/Core/Logger.h>
#include <Engine/Core/Asserts.h>
//#include <laml/laml.hpp>

global_variable bool32 GlobalRunning;
global_variable bool32 GlobalPause;
global_variable win32_state GlobalWin32State;
global_variable int GlobalCurrentInputIndex;
global_variable int GlobalGameUpdateHz;
global_variable int GlobalMonitorRefreshHz;
global_variable real32 GlobalTargetSecondsElapsedPerFrame;
global_variable game_input GlobalInput[2];

X_INPUT_GET_STATE(XInputGetStateSub) {
    return ERROR_DEVICE_NOT_CONNECTED;
}
X_INPUT_SET_STATE(XInputSetStateSub) {
    return ERROR_DEVICE_NOT_CONNECTED;
}

global_variable x_input_get_state *XInputGetState_ = XInputGetStateSub;
global_variable x_input_set_state *XInputSetState_ = XInputSetStateSub;

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_


internal_func void
    CatStrings(size_t SourceACount, char *SourceA,
               size_t SourceBCount, char *SourceB,
               size_t DestCount, char *Dest) {
    // TODO: Dest bounds checking!
    
    for(unsigned int Index = 0; Index < SourceACount; ++Index) {
        *Dest++ = *SourceA++;
    }

    for(unsigned int Index = 0; Index < SourceBCount; ++Index) {
        *Dest++ = *SourceB++;
    }

    *Dest++ = 0;
}

internal_func int 
    StringLength(char* String) {
    int Count = 0;
    while(*String++) {
        ++Count;
    }
    return Count;
}

internal_func void 
    Win32GetResourcePath(char* ResourcePathPrefix, uint8 PrefixLength) {
    if (PrefixLength) {
        // change all '\\' to '/'
        for (char* Scan = ResourcePathPrefix; *Scan; Scan++) {
            if (*Scan == '\\') {
                *Scan = '/';
            }
        }
        if (ResourcePathPrefix[PrefixLength-1] != '/') {
            ResourcePathPrefix[PrefixLength] = '/';
            PrefixLength++;
        }

        CatStrings(PrefixLength, ResourcePathPrefix, 0, 0, PrefixLength, GlobalWin32State.ResourcePathPrefix);
        GlobalWin32State.ResourcePrefixLength = PrefixLength;
    }
}

internal_func win32_window_dimension
    Win32GetWindowDimension(HWND Window) {
    win32_window_dimension Result;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;

    return Result;
}

internal_func void 
    Win32BuildEXEPathFileName(char* FileName, int DestCount, char* Dest) {
    CatStrings(GlobalWin32State.OnePastLastSlashEXEFileName - GlobalWin32State.EXEFileName, GlobalWin32State.EXEFileName,
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

internal_func void
    Win32LoadXInput() {
    HMODULE XInputLibrary;
    
    XInputLibrary = LoadLibraryA("XInput1_4.dll");
    if (XInputLibrary) {
        RH_DEBUG("Linking to xinput1_4.dll\n");
        XInputGetState_ = (x_input_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState_ = (x_input_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
        return;
    }

    XInputLibrary = LoadLibraryA("XInput9_1_0.dll");
    if (XInputLibrary) {
        RH_DEBUG("Linking to XInput9_1_0.dll\n");
        XInputGetState_ = (x_input_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState_ = (x_input_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
        return;
    }

    XInputLibrary = LoadLibraryA("xinput1_3.dll");
    if (XInputLibrary) {
        RH_DEBUG("Linking to xinput1_3.dll\n");
        XInputGetState_ = (x_input_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState_ = (x_input_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
        return;
    }

    RH_WARN("Could not find any version of XInput.dll!\n");
}

GAME_UPDATE_AND_RENDER_FUNC(GameUpdateAndRenderStub) {}

internal_func win32_game_code
Win32LoadGameCode(char *SourceDLLName, char *TempDLLName, char* LockFileName) {
    win32_game_code Result = {};

    WIN32_FILE_ATTRIBUTE_DATA Ignored;
    if (GetFileAttributesExA(LockFileName, GetFileExInfoStandard, &Ignored) == FALSE) {
        Result.DLLLastWriteTime = Win32GetLastWriteTime(SourceDLLName);

        CopyFileA(SourceDLLName, TempDLLName, FALSE);
    
        Result.GameCodeDLL = LoadLibraryA(TempDLLName);
        if (Result.GameCodeDLL) {
            // Get the handover function
            game_update_and_render* GameFunc = (game_update_and_render*)GetProcAddress(Result.GameCodeDLL, "GameUpdateAndRender");

            if (GameFunc) {
                Result.GameUpdateAndRender = GameFunc;
                Result.IsValid = true;
            } else {
                Result.GameUpdateAndRender = GameUpdateAndRenderStub;
            }
        }
        else {
            RH_ERROR("Failed to load game code: LoadLibraryA failed!\n");
        }
    } else {
        RH_TRACE("Failed to load game code: lock still in place!\n");
    }

    return(Result);
}

internal_func void Win32UnloadGameCode(win32_game_code *GameCode) {
    if(GameCode->GameCodeDLL) {
        FreeLibrary(GameCode->GameCodeDLL);
        GameCode->GameCodeDLL = 0;
    }

    GameCode->IsValid = false;

    GameCode->GameUpdateAndRender = GameUpdateAndRenderStub;
}

internal_func void 
    Win32GetEXEFileName() {
    // NOTE: Never use MAX_PATH in code that is user-facing, because it
    // can be dangerous and lead to bad results.
    DWORD SizeOfFilename = GetModuleFileNameA(0, GlobalWin32State.EXEFileName, sizeof(GlobalWin32State.EXEFileName));
    GlobalWin32State.OnePastLastSlashEXEFileName = GlobalWin32State.EXEFileName;
    for(char *Scan = GlobalWin32State.EXEFileName; *Scan; ++Scan) {
        if(*Scan == '\\') {
            GlobalWin32State.OnePastLastSlashEXEFileName = Scan + 1;
        }
    }
}

internal_func bool32 
    Win32Init(LPSTR CommandLine) {
    GlobalWin32State.WindowPosition.length = sizeof(GlobalWin32State.WindowPosition);

    //GlobalWin32State.Input[2] = {};

    LARGE_INTEGER PerfCounterFrequencyResult;
    QueryPerformanceFrequency(&PerfCounterFrequencyResult);
    GlobalWin32State.PerfCounterFrequency = PerfCounterFrequencyResult.QuadPart;

    Win32GetEXEFileName();

    // parse CommandLine to get a resource path
    bool32 FoundResourcePath = false;
    char ResourcePathPrefix[256] = { 0 };
    uint8 PrefixLength = 0;
    for (char* Scan = CommandLine; *Scan; Scan++) {
        if ((*Scan == '-') && (*(Scan+1) == 'r')) {
            FoundResourcePath = true;
            char* cpy = ResourcePathPrefix;
            for (char* Scan2 = (Scan + 3); (*Scan2 && (*Scan2 != ' ')); Scan2++) {
                *cpy++ = *Scan2;
                Scan = Scan2;
                PrefixLength++;
            }
        }
    }
    if (FoundResourcePath) {
        // .exe run with a -r flag
        RH_DEBUG("Run with a -r flag\n");
        Win32GetResourcePath(ResourcePathPrefix, PrefixLength);
    } else {
        DWORD dwAttrib = GetFileAttributes("Data");

        if (dwAttrib != INVALID_FILE_ATTRIBUTES && 
                        (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
            // the executable is right next to the data folder, no resource prefix needed
            RH_DEBUG("Found the Data/ directory!\n");
        } else {
            // Neither is the case, assuming we are being run in the build/ directory
            dwAttrib = GetFileAttributes("../Game");
            if (dwAttrib != INVALID_FILE_ATTRIBUTES &&
                            (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
                RH_DEBUG("Looks like you ran from the build/ directory!\n");
                char DefaultResourcePath[] = "../Game/run_tree/";
                Win32GetResourcePath(DefaultResourcePath, sizeof(DefaultResourcePath)-1);
            } else {
                RH_FATAL("I don't know where I am!\n");
                RH_FATAL("Shutting down...!\n");
                Sleep(2500);
                return false;
            }
        }
    }

    return true;
}

internal_func int
Win32GetMonitorRefreshRate(HWND Window) {
    int MonitorRefreshHz = 0;
    HMONITOR CurrentMonitor = MonitorFromWindow(Window, MONITOR_DEFAULTTONEAREST);
    MONITORINFOEXA MonitorInfo;
    MonitorInfo.cbSize = { sizeof(MONITORINFOEX) };
    if (GetMonitorInfoA(CurrentMonitor, &MonitorInfo)) {
        DEVMODEA MonitorDeviceMode = { sizeof(DEVMODEA) };
        if (EnumDisplaySettingsA(MonitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &MonitorDeviceMode)) {
            MonitorRefreshHz = MonitorDeviceMode.dmDisplayFrequency;
        }
    }
    return MonitorRefreshHz;
}

internal_func void 
    Win32ToggleFullscreen(HWND Window) {
    // TODO: Look into ChangeDisplaySettings function to change monitor refresh rate/resolution
    DWORD Style = GetWindowLong(Window, GWL_STYLE);
    if (Style & WS_OVERLAPPEDWINDOW) {
        MONITORINFO MonitorInfo = { sizeof(MonitorInfo) };
        if (GetWindowPlacement(Window, &GlobalWin32State.WindowPosition) &&GetMonitorInfo(MonitorFromWindow(Window, MONITOR_DEFAULTTOPRIMARY), &MonitorInfo)) {
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
        SetWindowPlacement(Window, &GlobalWin32State.WindowPosition);
        SetWindowPos(Window, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

internal_func void
    Win32DisplayBufferToWindow(HDC DeviceContext, int WindowWidth, int WindowHeight) {

    //SwapBuffers(DeviceContext);

#if 0
    if (GlobalWin32State.InputRecordingIndex == 1) {
        glFinish();
        // Draw text
        TCHAR text[ ] = "Recording input:";
        TextOutA(DeviceContext,0,0,text, ARRAYSIZE(text));
    } else if (GlobalWin32State.InputPlayingIndex == 1) {
        glFinish();
        // Draw text
        TCHAR text[ ] = "Replaying input:";
        TextOutA(DeviceContext,0,16,text, ARRAYSIZE(text));
    }
#endif
}

internal_func void 
    Win32GetInputFileLocation(bool32 InputStream, int SlotIndex, int DestCount, char* Dest) {
    char Temp[64];
    wsprintf(Temp, "loop_edit_%d_%s.hmi", SlotIndex, InputStream ? "input" : "state");
    Win32BuildEXEPathFileName(Temp, DestCount, Dest);
}

internal_func win32_replay_buffer* 
    Win32GetReplayBuffer(int Index) {
    Assert(Index < ArrayCount(GlobalWin32State.ReplayBuffers));
    win32_replay_buffer* Result = &GlobalWin32State.ReplayBuffers[Index];
    return Result;
}

internal_func void
    Win32BeginRecordingInput(int InputRecordingIndex) {
    win32_replay_buffer* ReplayBuffer = Win32GetReplayBuffer(InputRecordingIndex);
    if (ReplayBuffer->MemoryBlock) {
        GlobalWin32State.InputRecordingIndex = InputRecordingIndex;

        char Filename[WIN32_STATE_FILE_NAME_COUNT];
        Win32GetInputFileLocation(true, InputRecordingIndex, sizeof(Filename), Filename);
        GlobalWin32State.RecordingHandle = CreateFileA(Filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
        
#if 0
        LARGE_INTEGER FilePosition;
        FilePosition.QuadPart = GlobalWin32State.TotalSize;
        SetFilePointerEx(GlobalWin32State.RecordingHandle, FilePosition, 0, FILE_BEGIN);
#endif
        CopyMemory(ReplayBuffer->MemoryBlock, GlobalWin32State.GameMemoryBlock, GlobalWin32State.TotalSize);
    }
}

internal_func void
    Win32EndRecordingInput() {
    CloseHandle(GlobalWin32State.RecordingHandle);
    GlobalWin32State.InputRecordingIndex = 0;
}

internal_func void
    Win32BeginInputPlayback(int InputPlayingIndex) {
    win32_replay_buffer* ReplayBuffer = Win32GetReplayBuffer(InputPlayingIndex);
    if (ReplayBuffer->MemoryBlock) {
        GlobalWin32State.InputPlayingIndex = InputPlayingIndex;

        char Filename[WIN32_STATE_FILE_NAME_COUNT];
        Win32GetInputFileLocation(true, InputPlayingIndex, sizeof(Filename), Filename);
        GlobalWin32State.PlaybackHandle = CreateFileA(Filename, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);

#if 0
        LARGE_INTEGER FilePosition;
        FilePosition.QuadPart = GlobalWin32State.TotalSize;
        SetFilePointerEx(GlobalWin32State.PlaybackHandle, FilePosition, 0, FILE_BEGIN);
#endif

        CopyMemory(GlobalWin32State.GameMemoryBlock, ReplayBuffer->MemoryBlock, GlobalWin32State.TotalSize);
    }
}

internal_func void
    Win32EndInputPlayback() {
    CloseHandle(GlobalWin32State.PlaybackHandle);
    GlobalWin32State.InputPlayingIndex = 0;
}

internal_func void 
    Win32RecordInput(game_input* NewInput) {
    DWORD BytesWritten;
    WriteFile(GlobalWin32State.RecordingHandle, NewInput, sizeof(*NewInput), &BytesWritten, 0);
}

internal_func void 
    Win32PlaybackInput(game_input* NewInput) {
    DWORD BytesRead;
    if (ReadFile(GlobalWin32State.PlaybackHandle, NewInput, sizeof(*NewInput), &BytesRead, 0)) {
        // There's still input
        if (BytesRead == 0) {
            // Looping the stream
            RH_DEBUG("Looping input recording...\n");
            int PlayingIndex = GlobalWin32State.InputPlayingIndex; 
            Win32EndInputPlayback();
            Win32BeginInputPlayback(PlayingIndex);
            ReadFile(GlobalWin32State.PlaybackHandle, NewInput, sizeof(*NewInput), &BytesRead, 0);
        }
    } else {
    }
}

internal_func void
    Win32ProcessKeyboardMessage(game_button_state* NewState,
                                bool32 IsDown) {
    if (NewState->EndedDown != IsDown) {
        NewState->EndedDown = IsDown;
        NewState->HalfTransitionCount++;
    }
}

LRESULT CALLBACK
    Win32MainWindowCallback(HWND Window,
                            UINT Message,
                            WPARAM WParam,
                            LPARAM LParam) {

    LRESULT Result = 0;

    game_input* CurrentInput = &GlobalInput[GlobalCurrentInputIndex];

    switch (Message) {
        case WM_MOVE: {
            if (GlobalTargetSecondsElapsedPerFrame > 0) {
                int NewRefreshHz = Win32GetMonitorRefreshRate(Window);
                if (NewRefreshHz != GlobalMonitorRefreshHz) {
                    RH_INFO("New monitor Refresh Hz [%d] => [%d]\n", GlobalMonitorRefreshHz, NewRefreshHz);
                    GlobalMonitorRefreshHz = NewRefreshHz;
                    GlobalGameUpdateHz = 60;
                    //Win32UpdateVSync(&GlobalMonitorRefreshHz, &GlobalGameUpdateHz);
                    GlobalTargetSecondsElapsedPerFrame = 1.0f / (real32)GlobalGameUpdateHz;
                }
            }
        } break;
        case WM_QUIT: {
            GlobalRunning = false;
        } break;

        case WM_SIZE: {
            RH_TRACE("WM_SIZE\n");
        } break;

        case WM_CLOSE: {
            RH_TRACE("WM_CLOSE\n");

            //TODO: Handle this w/ msg to user
            GlobalRunning = false;
        } break;

        case WM_ACTIVATEAPP: {
#if 0
            if (WParam == TRUE) {
                SetLayeredWindowAttributes(Window,RGB(0, 0, 0),255,LWA_ALPHA);
            } else {
                SetLayeredWindowAttributes(Window,RGB(0, 0, 0),128,LWA_ALPHA);
            }
#endif
            RH_TRACE("WM_ACTIVATEAPP\n");
        } break;

        case WM_DESTROY: {
            RH_TRACE("WM_DESTROY\n");

            //TODO: Handle this as an error - recreate window?
            GlobalRunning = false;
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            //game_controller_input* KeyboardController = GetController(CurrentInput, 0);
            game_controller_input* KeyboardController = &CurrentInput->Controllers[0];

            uint32 VKCode = (uint32)WParam;
            bool32 WasDown = ((LParam & (1 << 30)) != 0);
            bool32 IsDown = ((LParam & (1 << 31)) == 0);
            bool32 AltKeyWasDown = (LParam & (1 << 29));

            if (WasDown != IsDown) {
                if (0) {
                }
                else if (VKCode == 'W') {
                    Win32ProcessKeyboardMessage(&KeyboardController->MoveUp, IsDown);
                } 
#if 0
                else if (VKCode == 'A') {
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
                    GlobalRunning = false;
                    Win32ProcessKeyboardMessage(&KeyboardController->Start, IsDown);
                } else if (VKCode == VK_SPACE) {
                    Win32ProcessKeyboardMessage(&KeyboardController->Back, IsDown);
                }
#endif
#if RH_INTERNAL
                else if (VKCode == 'P') {
                    if (IsDown)
                        GlobalPause = !GlobalPause;
                }
                else if(VKCode == 'L') {
                    if (IsDown) {
                        if (GlobalWin32State.InputPlayingIndex == 0) {
                            if (GlobalWin32State.InputRecordingIndex == 0) {
                                Win32BeginRecordingInput(1);
                            } else {
                                Win32EndRecordingInput();
                                Win32BeginInputPlayback(1);
                            }
                        } else {
                            Win32EndInputPlayback();
                        }
                    }
                }
#endif
                else if ((VKCode == VK_F4) && AltKeyWasDown) {
                    GlobalRunning = false;
                } else if ((VKCode == VK_RETURN) && AltKeyWasDown) {
                    if (IsDown) {
                        if (Window) {
                            Win32ToggleFullscreen(Window);
                        }
                    }
                }
            }
        } break;

        case WM_LBUTTONDOWN: { 
            RH_TRACE("Mouse1 Down\n"); 
            //Win32ProcessKeyboardMessage(&Input->MouseButtons[0], true);
        } break;
        case WM_LBUTTONUP: { 
            RH_TRACE("Mouse1 Up\n"); 
            //Win32ProcessKeyboardMessage(&Input->MouseButtons[0], false);
        } break;

        case WM_RBUTTONDOWN: { 
            RH_TRACE("Mouse2 Down\n"); 
            //Win32ProcessKeyboardMessage(&Input->MouseButtons[1], true);
        } break;
        case WM_RBUTTONUP: { 
            RH_TRACE("Mouse2 Up\n"); 
            //Win32ProcessKeyboardMessage(&Input->MouseButtons[1], false);
        } break;

        case WM_MBUTTONDOWN: { 
            RH_TRACE("Mouse3 Down\n"); 
            //Win32ProcessKeyboardMessage(&Input->MouseButtons[2], true);
        } break;
        case WM_MBUTTONUP: { 
            RH_TRACE("Mouse3 Up\n"); 
            //Win32ProcessKeyboardMessage(&Input->MouseButtons[2], false);
        } break;

        case WM_XBUTTONDOWN: { 
            bool32 IsXButton1 = (WParam & 0x00010000);
            bool32 IsXButton2 = (WParam & 0x00020000);

            if (IsXButton1) {
                RH_TRACE("Mouse4 Down\n");
                //Win32ProcessKeyboardMessage(&Input->MouseButtons[3], true);
            }
            else if (IsXButton2) {
                RH_TRACE("Mouse5 Down\n");
                //Win32ProcessKeyboardMessage(&Input->MouseButtons[4], true);
            }
        } break;
        case WM_XBUTTONUP: { 
            bool32 IsXButton1 = (WParam & 0x00010000);
            bool32 IsXButton2 = (WParam & 0x00020000);

            if (IsXButton1) {
                RH_TRACE("Mouse4 Up\n");
                //Win32ProcessKeyboardMessage(&Input->MouseButtons[3], false);
            }
            else if (IsXButton2) {
                RH_TRACE("Mouse5 Up\n");
                //Win32ProcessKeyboardMessage(&Input->MouseButtons[4], false);
            }
        } break;

        case WM_PAINT: {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            win32_window_dimension Dimension = Win32GetWindowDimension(Window);
            Win32DisplayBufferToWindow(DeviceContext, Dimension.Width, Dimension.Height);

            EndPaint(Window, &Paint);
        } break;

        default: {
            //OutputDebugString("default\n");
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }

    return Result;
}

inline LARGE_INTEGER
    Win32GetWallClock() {
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);

    return Result;
}

inline real32
    Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End) {
    real32 Result = ((real32)(End.QuadPart - Start.QuadPart)) / (real32)GlobalWin32State.PerfCounterFrequency;
    return Result;
}


internal_func void 
    Win32GetSystemSpecs() {
    DWORD buffer_size = 0;
    DWORD i = 0;
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION * buffer = 0;

    GetLogicalProcessorInformation(0, &buffer_size);
    buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)malloc(buffer_size);
    GetLogicalProcessorInformation(&buffer[0], &buffer_size);

    size_t L1LineSize = 0;
    size_t L2LineSize = 0;
    size_t L3LineSize = 0;
    for (i = 0; i != buffer_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
        if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1) {
            L1LineSize = buffer[i].Cache.LineSize;
            //break;
        }
        if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 2) {
            L2LineSize = buffer[i].Cache.LineSize;
            //break;
        }
        if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 3) {
            L3LineSize = buffer[i].Cache.LineSize;
            //break;
        }
    }

    free(buffer);

    RH_DEBUG("L1 Cache Line Size: %u bytes\n", L1LineSize);
    RH_DEBUG("L2 Cache Line Size: %u bytes\n", L2LineSize);
    RH_DEBUG("L3 Cache Line Size: %u bytes\n", L3LineSize);
}










int main(int argc, char** argv) {

    HINSTANCE Instance = GetModuleHandleA(NULL);
    LPSTR CommandLine = GetCommandLineA();

    if (!Win32Init(CommandLine)) {
        return -1;
    }

    InitLogging();

    // NOTE: Set the Windows scheduler granularity to 1ms
    //       so that our Sleep can be more granular
    UINT DesiredSchedulerMS = 1;
    if (!(timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR)) {
        // ERROR!
        OutputDebugStringA("Could not set Windows scheduler granularity to 1ms!");
    }

    Win32LoadXInput();

    Win32GetSystemSpecs();

    char SourceGameCodeDLLFullPath[WIN32_STATE_FILE_NAME_COUNT];
    Win32BuildEXEPathFileName("game.dll", sizeof(SourceGameCodeDLLFullPath), SourceGameCodeDLLFullPath);

    char TempGameCodeDLLFullPath[WIN32_STATE_FILE_NAME_COUNT];
    Win32BuildEXEPathFileName("game_temp.dll", sizeof(TempGameCodeDLLFullPath), TempGameCodeDLLFullPath);

    char GameCodeLockFullPath[WIN32_STATE_FILE_NAME_COUNT];
    Win32BuildEXEPathFileName("lock.tmp", sizeof(GameCodeLockFullPath), GameCodeLockFullPath);

    WNDCLASSA WindowClass = {};

    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = LoadCursorA(NULL, IDC_CROSS);
    //WindowClass.hIcon;
    WindowClass.lpszClassName = "RhWindowClass";

    if (RegisterClassA(&WindowClass)) {
        HWND Window = CreateWindowExA(
                      0, //WS_EX_TOPMOST|WS_EX_LAYERED, 
            WindowClass.lpszClassName, "Rohin",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            0, 0, Instance, 0);
        if (Window) {
            HDC DeviceContext = GetDC(Window);

            // How do we reliably query this on Windows?
            GlobalMonitorRefreshHz = 60;
            GlobalGameUpdateHz = 60;
            GlobalMonitorRefreshHz = Win32GetMonitorRefreshRate(Window);

            //Win32InitOpenGL(Window, &GlobalMonitorRefreshHz, &GlobalGameUpdateHz);
            //InitDebugRenderState();

            GlobalTargetSecondsElapsedPerFrame = 1.0f / (real32)GlobalGameUpdateHz;

            GlobalRunning = true;

#if RH_INTERNAL
            uint64 BaseAddress = Terabytes(2);
#else
            uint64 BaseAddress = 0;
#endif
            game_memory GameMemory = {};
            GameMemory.PermanentStorageSize = Megabytes(64);
            GameMemory.TransientStorageSize = Gigabytes(1);

            GlobalWin32State.TotalSize = GameMemory.PermanentStorageSize + GameMemory.TransientStorageSize;
            // TODO: look into MEM_LARGE_PAGES
            GlobalWin32State.GameMemoryBlock = VirtualAlloc((LPVOID)BaseAddress, (size_t)GlobalWin32State.TotalSize,
                                                            MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            GameMemory.PermanentStorage = GlobalWin32State.GameMemoryBlock;
            GameMemory.TransientStorage = ((uint8*)GameMemory.PermanentStorage + GameMemory.PermanentStorageSize);

            
            uint64 RenderStorageSize = Megabytes(2);
            void*  RenderStorage = VirtualAlloc((LPVOID)(BaseAddress + GlobalWin32State.TotalSize), (size_t)RenderStorageSize,
                                               MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            //GlobalCommandBuffer.MaxSize = (uint32)RenderStorageSize;
            //GlobalCommandBuffer.Base = (uint8*)RenderStorage;

#if RH_INTERNAL
            for (int ReplayIndex = 0; ReplayIndex < ArrayCount(GlobalWin32State.ReplayBuffers); ReplayIndex++) {
                win32_replay_buffer* ReplayBuffer = &GlobalWin32State.ReplayBuffers[ReplayIndex];

                // TODO: These files should go to a tmp directory!
                Win32GetInputFileLocation(false, ReplayIndex, sizeof(ReplayBuffer->ReplayFilename), ReplayBuffer->ReplayFilename);

                ReplayBuffer->FileHandle = CreateFileA(ReplayBuffer->ReplayFilename, GENERIC_WRITE|GENERIC_READ, 0, 0, CREATE_ALWAYS, 0, 0);

                LARGE_INTEGER MaxSize;
                MaxSize.QuadPart = GlobalWin32State.TotalSize;
                ReplayBuffer->MemoryMap = CreateFileMappingA(
                                          ReplayBuffer->FileHandle, 0, 
                    PAGE_READWRITE, 
                    MaxSize.HighPart, MaxSize.LowPart, 0);

                ReplayBuffer->MemoryBlock = MapViewOfFile(ReplayBuffer->MemoryMap, FILE_MAP_ALL_ACCESS, 0, 0, GlobalWin32State.TotalSize);

                if(!ReplayBuffer->MemoryBlock) {
                    // TODO: Diagnostic
                }
            }
#endif

            if (GameMemory.PermanentStorage && GameMemory.TransientStorage) {

                LARGE_INTEGER LastCounter = Win32GetWallClock();
                LARGE_INTEGER FlipWallClock = Win32GetWallClock();

                uint64 LastCycleCount = __rdtsc();

                win32_game_code Game = Win32LoadGameCode(SourceGameCodeDLLFullPath,
                                                         TempGameCodeDLLFullPath,
                                                         GameCodeLockFullPath);
                if (!Game.IsValid) {
                    MessageBoxA(Window, "Could not find/open game.dll to run!\nExiting now...", "Failed to find game.dll", MB_OK | MB_ICONERROR);
                    GlobalRunning = false;
                }

                GlobalCurrentInputIndex = 0;
                game_input* NewInput = &GlobalInput[0];
                game_input* OldInput = &GlobalInput[1];

                //MovingAverage<real32, 30> MSLastFrame;
                //MovingAverage<real32, 30> MSWorkLastFrame;

                win32_window_dimension LastDimension = Win32GetWindowDimension(Window);

                while (GlobalRunning) {
                    NewInput->dtForFrame = GlobalTargetSecondsElapsedPerFrame;

                    FILETIME NewDLLWriteTime = Win32GetLastWriteTime(SourceGameCodeDLLFullPath);
                    if (CompareFileTime(&NewDLLWriteTime, &Game.DLLLastWriteTime) != 0) {
                        Win32UnloadGameCode(&Game);
                        Game = Win32LoadGameCode(SourceGameCodeDLLFullPath,
                                                 TempGameCodeDLLFullPath,
                                                 GameCodeLockFullPath);
                    }

#if 0
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
#endif
                    for (int ButtonIndex = 0; ButtonIndex < ArrayCount(NewInput->MouseButtons); ButtonIndex++) {
                        NewInput->MouseButtons[ButtonIndex].EndedDown =
                            OldInput->MouseButtons[ButtonIndex].EndedDown;
                    }

                    MSG Message;
                    while (PeekMessageA(&Message, NULL, 0, 0, PM_REMOVE)) {
                        TranslateMessage(&Message);
                        DispatchMessageA(&Message);
                    }

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
                            //game_controller_input *OldController = GetController(OldInput, OurControllerIndex);
                            //game_controller_input *NewController = GetController(NewInput, OurControllerIndex);

                            XINPUT_STATE ControllerState;
                            if (XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS) {
                                // this controller is plugged in!
                                XINPUT_GAMEPAD* Pad = &ControllerState.Gamepad;

                            } else {
                                // This controller is not available!
                                //NewController->IsConnected = false;
                            }
                        }

                        //XINPUT_VIBRATION Vibration;
                        //Vibration.wLeftMotorSpeed  = 20000;
                        //Vibration.wRightMotorSpeed = 20000;
                        //XInputSetState(0, &Vibration);

                        if (GlobalWin32State.InputRecordingIndex) {
                            Win32RecordInput(NewInput);
                        }

                        if (GlobalWin32State.InputPlayingIndex) {
                            Win32PlaybackInput(NewInput);
                        }
                        win32_window_dimension Dimension = Win32GetWindowDimension(Window);
                        if ((Dimension.Width != LastDimension.Width) || (Dimension.Height != LastDimension.Height)) {
                            //laml::transform::create_projection_orthographic(GlobalDebugRenderState.DebugTextRenderer.orthoMat, 0.0f, (real32)Dimension.Width, (real32)Dimension.Height, 0.0f, -1.0f, 1.0f);
                        }

                        //OpenGLBeginFrame();
                        Game.GameUpdateAndRender(&GameMemory, NewInput); //, &GlobalCommandBuffer);
                        //Win32ListRenderCommands(&CommandBuffer);

#if 0
                        if (GlobalRenderDebugText) {
                            char DebugFrameTimeBuff[64];
                            rh::laml::Vec3 DebugColor(0.85f, .65f, .6f);
                            rh::laml::Vec3 DebugColorBack(0.3f, .12f, .05f);
                            real32 offset = 1.0f;

                            real32 AvgFrameTime = MSLastFrame.getCurrentAverage();
                            real32 AvgFrameRate = 1000.0f / AvgFrameTime;
                        
                            StringCbPrintfA(DebugFrameTimeBuff, sizeof(DebugFrameTimeBuff),
                                            "Last frame time: %.02f ms", AvgFrameTime);
                            DrawDebugText(DebugFrameTimeBuff, (real32)Dimension.Width, 0.0f, DebugColorBack, TextAlignment::ALIGN_TOP_RIGHT);
                            DrawDebugText(DebugFrameTimeBuff, (real32)Dimension.Width+offset, offset, DebugColor, TextAlignment::ALIGN_TOP_RIGHT);

                            StringCbPrintfA(DebugFrameTimeBuff, sizeof(DebugFrameTimeBuff),
                                            "Work done: %.02f ms", MSWorkLastFrame.getCurrentAverage());
                            DrawDebugText(DebugFrameTimeBuff, (real32)Dimension.Width, 24.0f, DebugColorBack, TextAlignment::ALIGN_TOP_RIGHT);
                            DrawDebugText(DebugFrameTimeBuff, (real32)Dimension.Width+offset, 24.0f+offset, DebugColor, TextAlignment::ALIGN_TOP_RIGHT);

                            StringCbPrintfA(DebugFrameTimeBuff, sizeof(DebugFrameTimeBuff),
                                            "%.0f fps", AvgFrameRate);
                            DrawDebugText(DebugFrameTimeBuff, (real32)Dimension.Width, 48.0f, DebugColorBack, TextAlignment::ALIGN_TOP_RIGHT);
                            DrawDebugText(DebugFrameTimeBuff, (real32)Dimension.Width+offset, 48.0f+offset, DebugColor, TextAlignment::ALIGN_TOP_RIGHT);


                            StringCbPrintfA(DebugFrameTimeBuff, sizeof(DebugFrameTimeBuff),
                                            "Render Commands: %d (%dKb/%dMb)", 
                                            GlobalCommandBuffer.ElementCount, 
                                            GlobalCommandBuffer.CurrentSize/Kilobytes(1), GlobalCommandBuffer.MaxSize/Megabytes(1));
                            DrawDebugText(DebugFrameTimeBuff, (real32)Dimension.Width, 72.0f, DebugColorBack, TextAlignment::ALIGN_TOP_RIGHT);
                            DrawDebugText(DebugFrameTimeBuff, (real32)Dimension.Width+offset, 72.0f+offset, DebugColor, TextAlignment::ALIGN_TOP_RIGHT);
                        }
#endif
                        
                        //OpenGLEndFrame(&Dimension, &GlobalCommandBuffer);




                        LARGE_INTEGER WorkCounter = Win32GetWallClock();
                        real32 WorkSecondsElapsed = Win32GetSecondsElapsed(LastCounter, WorkCounter);
                        //MSWorkLastFrame.addSample(WorkSecondsElapsed*1000.0f);

                        // TODO: Untested! buggy
                        real32 SecondsElapsedForFrame = WorkSecondsElapsed;
                        bool32 LockFramerate = true;
                        if (LockFramerate) {
                            if (SecondsElapsedForFrame < GlobalTargetSecondsElapsedPerFrame) {
                                DWORD SleepMS = (DWORD)(1000.0f*(GlobalTargetSecondsElapsedPerFrame - SecondsElapsedForFrame));
                                if (SleepMS > 0) {
                                    Sleep(SleepMS);
                                }
    
                                while (SecondsElapsedForFrame < GlobalTargetSecondsElapsedPerFrame) {
                                    SecondsElapsedForFrame = Win32GetSecondsElapsed(LastCounter, Win32GetWallClock());
                                }
                            } else {
                                // TODO: Missed Frame Rate!
                                RH_ERROR("Frame rate missed!\n");
                            }
                        }
    
                        LARGE_INTEGER EndCounter = Win32GetWallClock();
                        real32 MSPerFrame = 1000.0f * Win32GetSecondsElapsed(LastCounter, EndCounter);
                        //MSLastFrame.addSample(MSPerFrame);
                        LastCounter = EndCounter;
    
                        Win32DisplayBufferToWindow(DeviceContext, Dimension.Width, Dimension.Height);
    
                        FlipWallClock = Win32GetWallClock();
    
                        
                        GlobalCurrentInputIndex = (GlobalCurrentInputIndex) ? 0 : 1;
                        game_input* Temp = NewInput;
                        NewInput = OldInput;
                        OldInput = Temp;
    
#if 1
                        uint64 EndCycleCount = __rdtsc();
                        uint64 CyclesElapsed = EndCycleCount - LastCycleCount;
                        LastCycleCount = EndCycleCount;
    
                        real32 FPS = 1000.0f / MSPerFrame;
                        real32 MCPF = ((real32)CyclesElapsed / (1000.0f*1000.0f));
    
                        //Win32LogMessage("Frame: %.02f ms  %.02ffps   %.02f MHz\n", MSPerFrame, FPS, MCPF);
                        char FPSBuffer[256];
                        StringCbPrintfA(FPSBuffer, sizeof(FPSBuffer), 
                                        "Frame: %.02f ms  %.02ffps   %.02f MHz\n", MSPerFrame, FPS, MCPF);
                        OutputDebugStringA(FPSBuffer);
#endif
                    }
                }
            } else {
                LogOutput(LOG_LEVEL_FATAL, "Failed to acquire memory!");
            }
        } else {
            LogOutput(LOG_LEVEL_FATAL, "Failed to create window");
        }
    } else {
        LogOutput(LOG_LEVEL_FATAL, "Failed to register window class");
    }

    ShutdownLogging();

    return 0;
}


























#if 0

                        

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






                            




                    





































#include "win32_entry.h"

#include <Engine/Core/Asserts.h>

inline game_controller_input*
    GetController(game_input* Input, int ControllerIndex) {
    Assert(ControllerIndex < ArrayCount(Input->Controllers));

    game_controller_input* Controller = &Input->Controllers[ControllerIndex];
    return Controller;
}




internal_func void
Win32ProcessXInputDigitalButton(DWORD XInputButtonState,
                                    game_button_state* OldState,
                                    DWORD ButtonBit,
                                    game_button_state* NewState) {

    NewState->EndedDown = (XInputButtonState & ButtonBit) == ButtonBit;
    NewState->HalfTransitionCount = (OldState->EndedDown == NewState->EndedDown) ? 1 : 0;
}



internal_func real32
Win32ProcessXInputStickValue(SHORT StickValue, SHORT DeadzoneValue) {
    real32 Value = 0;

    if (StickValue < -DeadzoneValue) {
        Value = (real32)(StickValue + DeadzoneValue) / (32768.0f - DeadzoneValue);
    } else if (StickValue > DeadzoneValue) {
        Value = (real32)(StickValue - DeadzoneValue) / (32767.0f - DeadzoneValue);
    }
    return Value;
}

#if 0
bool32 Win32LoadShaderFromFile(shader* shader, char* ResourcePath) {
    bool32 Result = false;
    char ShaderFullPath[WIN32_STATE_FILE_NAME_COUNT];
    CatStrings(GlobalWin32State.ResourcePrefixLength, GlobalWin32State.ResourcePathPrefix, 
               StringLength(ResourcePath), ResourcePath, 
               sizeof(ShaderFullPath), ShaderFullPath);

    HANDLE FileHandle = CreateFileA(ShaderFullPath, 
                                    GENERIC_READ, FILE_SHARE_READ, NULL, 
                                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE != FileHandle) {
        LARGE_INTEGER FileSizeBytes;
        if (GetFileSizeEx(FileHandle, &FileSizeBytes)) {

            // use FileSizeBytes + 1 to allow a null-terminator to be added
            FileSizeBytes.QuadPart++;
            LPVOID Buffer = VirtualAlloc(0, FileSizeBytes.QuadPart,
                         MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

            DWORD BytesRead;
            if (Buffer) {
                if (ReadFile(FileHandle, Buffer, (DWORD)FileSizeBytes.QuadPart, &BytesRead, NULL)) {
                    ((uint8*)Buffer)[BytesRead] = 0;
                    // read the entire file into a buffer
                    Win32LogMessage("Compiling OpenGL Shader: %s\n", ResourcePath);
                    Result = OpenGLLoadShader(shader, (uint8*)Buffer, BytesRead);
                
                    CloseHandle(FileHandle);
                }
                VirtualFree(Buffer, 0, MEM_RELEASE);
            }
        }

    }

    return Result;
}

uint8* Win32ReadEntireFile(memory_arena* Arena, char* ResourcePath, uint32 *BytesRead) {
    uint8* Result = nullptr;

    char ResourceFullPath[WIN32_STATE_FILE_NAME_COUNT];
    CatStrings(GlobalWin32State.ResourcePrefixLength, GlobalWin32State.ResourcePathPrefix, 
               StringLength(ResourcePath), ResourcePath, 
               sizeof(ResourceFullPath), ResourceFullPath);

    HANDLE FileHandle = CreateFileA(ResourceFullPath, 
                                    GENERIC_READ, FILE_SHARE_READ, NULL, 
                                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE != FileHandle) {
        LARGE_INTEGER FileSizeBytes;
        if (GetFileSizeEx(FileHandle, &FileSizeBytes)) {

            LPVOID Buffer = PushSize_(Arena, FileSizeBytes.QuadPart);

            DWORD Win32BytesRead;
            if (Buffer) {
                if (ReadFile(FileHandle, Buffer, (DWORD)FileSizeBytes.QuadPart, &Win32BytesRead, NULL)) {
                    Assert(Win32BytesRead == FileSizeBytes.QuadPart);
                    Result = (uint8*)Buffer;
                    *BytesRead = Win32BytesRead;
                }
            }
        }
        CloseHandle(FileHandle);
    }

    return Result;
}

bool32 Win32LoadTextureFromFile(texture_2D* Texture, char* ResourcePath) {
    // TODO: allocations here are yucky!

    char TextureFullPath[WIN32_STATE_FILE_NAME_COUNT];
    CatStrings(GlobalWin32State.ResourcePrefixLength, GlobalWin32State.ResourcePathPrefix, 
               StringLength(ResourcePath), ResourcePath, 
               sizeof(TextureFullPath), TextureFullPath);

    stbi_set_flip_vertically_on_load(true);
    int Width, Height, NumChannels;
    stbi_uc* data = stbi_load(TextureFullPath, &Width, &Height, &NumChannels, 0);
    Assert(data);

    OpenGLCreateTexture(Texture, data, Texture->Width, Texture->Height, Texture->NumChannels);

    stbi_image_free(data);

    return true;
}

bool32 Win32LoadDynamicFont(dynamic_font* Font, char* ResourcePath, real32 FontSize, uint16 Resolution) {
    bool32 Result = false;

    // TODO: allocations here are yucky
    SIZE_T BufferSize = Resolution * Resolution * sizeof(unsigned char);
    LPVOID BitmapBuffer = VirtualAlloc(0, BufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (BitmapBuffer) {
        uint8* temp_bitmap = (uint8* )BitmapBuffer;

        char FontFullPath[WIN32_STATE_FILE_NAME_COUNT];
        CatStrings(GlobalWin32State.ResourcePrefixLength, GlobalWin32State.ResourcePathPrefix, 
                   StringLength(ResourcePath), ResourcePath, 
                   sizeof(FontFullPath), FontFullPath);

        HANDLE FileHandle = CreateFileA(FontFullPath, 
                                        GENERIC_READ, FILE_SHARE_READ, NULL, 
                                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (INVALID_HANDLE_VALUE != FileHandle) {
            LARGE_INTEGER FileSizeBytes;
            if (GetFileSizeEx(FileHandle, &FileSizeBytes)) {

                Assert(FileSizeBytes.QuadPart < Megabytes(1));
                LPVOID Buffer = VirtualAlloc(0, FileSizeBytes.QuadPart,
                                             MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

                if (Buffer) {
                    DWORD BytesRead;
                    if (ReadFile(FileHandle, ttf_buffer, (DWORD)FileSizeBytes.QuadPart, &BytesRead, NULL)) {
                        // read the entire file into a buffer
                    
                        int STBResult = stbtt_BakeFontBitmap(ttf_buffer, 0, FontSize, temp_bitmap, Resolution, Resolution, 32, 96, &Font->cdata[0]); // no guarantee this fits!
                        if (STBResult != -1) {
                            Result = true;

                            OpenGLCreateTexture(&Font->Texture, temp_bitmap, Resolution, Resolution, 1);
                            Font->BitmapRes = Resolution;
                            Font->FontSize = FontSize;
                            Font->Initialized = true;
                        }
                
                    }
                    VirtualFree(Buffer, 0, MEM_RELEASE);
                }
            }

            CloseHandle(FileHandle);
        }

        VirtualFree(BitmapBuffer, 0, MEM_RELEASE);
    }

    return Result;
}

internal_func void InitDebugRenderState() {
    // Init TextRenderer
    Win32LoadShaderFromFile((shader*)(&GlobalDebugRenderState.DebugTextRenderer.Shader), "Data/Shaders/debug/Text.glsl");
    GlobalDebugRenderState.DebugTextRenderer.Shader.InitShaderLocs();

    rh::laml::transform::create_projection_orthographic(GlobalDebugRenderState.DebugTextRenderer.orthoMat, 0.0f, 1424.0f, 720.0f, 0.0f, -1.0f, 1.0f);
    real32 QuadVerts[] = {
                         0.0f, 1.0f,
                         0.0f, 0.0f,
                         1.0f, 0.0f,
                         1.0f, 1.0f
    };
    uint32 QuadIndices[] = {
                           0, 1, 2,
                           0, 2, 3
    };
    vertex_buffer VBO = OpenGLCreateVertexBuffer(QuadVerts, sizeof(QuadVerts), 1, ShaderDataType::Float2);
    index_buffer IBO = OpenGLCreateIndexBuffer(QuadIndices, 6);
    GlobalDebugRenderState.DebugTextRenderer.TextQuad = OpenGLCreateVertexArray(&VBO, &IBO);

    Win32LoadDynamicFont(&GlobalDebugRenderState.DebugTextRenderer.Font, "Data/Fonts/UbuntuMono-Regular.ttf", 24.0f, 512);
}

internal_func void DrawDebugText(char* Text, real32 StartX, real32 StartY, rh::laml::Vec3 Color, TextAlignment Alignment) {
    render_command_buffer* CmdBuffer = &GlobalCommandBuffer;
    text_renderer* TextRenderer = &GlobalDebugRenderState.DebugTextRenderer;
    dynamic_font* Font = &TextRenderer->Font;

    real32 X = StartX;
    real32 Y = StartY;

    real32 HOffset, VOffset;
    GetTextOffset(&TextRenderer->Font, &HOffset, &VOffset, Alignment, Text);

    Render_BindShader(CmdBuffer, TextRenderer->Shader);
    Render_BindVAO(CmdBuffer, TextRenderer->TextQuad);
    Render_BindTexture(CmdBuffer, 0, TextRenderer->Font.Texture.Handle);
    Render_SetFrontCull(CmdBuffer, true);
    Render_SetDepthTest(CmdBuffer, false);
    Render_UploadInt(CmdBuffer, TextRenderer->Shader.r_fontTex, 0);
    Render_UploadVec3(CmdBuffer, TextRenderer->Shader.r_textColor, Color);
    Render_UploadMat4(CmdBuffer, TextRenderer->Shader.r_orthoProjection, TextRenderer->orthoMat);

    while (*Text) {
        if (*Text == '\n') {
            //Increase y by one line,
            //reset x to start
            X = StartX;
            Y += TextRenderer->Font.FontSize;
        }
        if (*Text >= 32 && *Text < 128) {
            stbtt_aligned_quad q;
            char c = *Text - 32;
            stbtt_GetBakedQuad((Font->cdata), Font->BitmapRes, Font->BitmapRes, *Text - 32, &X, &Y, &q, 1);//1=opengl & d3d10+,0=d3d9

            float scaleX = q.x1 - q.x0;
            float scaleY = q.y1 - q.y0;
            float transX = q.x0;
            float transY = q.y0;
            Render_UploadVec4(CmdBuffer, TextRenderer->Shader.r_transform, rh::laml::Vec4(scaleX, scaleY, transX + HOffset, transY + VOffset));

            scaleX = q.s1 - q.s0;
            scaleY = q.t1 - q.t0;
            transX = q.s0;
            transY = q.t0;
            Render_UploadVec4(CmdBuffer, TextRenderer->Shader.r_transformUV, rh::laml::Vec4(scaleX, scaleY, transX, transY));

            Render_Submit(CmdBuffer, TextRenderer->TextQuad.IndexCount);
        }
        ++Text;
    }

    Render_SetDepthTest(CmdBuffer, true);
    Render_SetFrontCull(CmdBuffer, false);
}
#endif







#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#endif


#endif