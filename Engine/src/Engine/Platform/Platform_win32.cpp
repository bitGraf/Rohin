#include "Platform.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"
#include "Engine/Core/Event.h"
#include "Engine/Memory/MemoryUtils.h"
#include "Engine/Core/Input.h"

#ifdef RH_PLATFORM_WINDOWS

#include <Windows.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <windowsx.h>
#include <stdarg.h>
#include <stdio.h>
//#include "Engine/Platform/WGL/win32_opengl.h"

#include "backends/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// https://learn.microsoft.com/en-us/windows/win32/dxtecharts/taking-advantage-of-high-dpi-mouse-movement?redirectedfrom=MSDN#wm_input
// you can #include <hidusage.h> for these defines
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

// platform_win32.cpp manages this struct, no one else needs to access it
#define WIN32_STATE_FILE_NAME_COUNT MAX_PATH
struct PlatformState {
    HINSTANCE instance;
    HWND window;
    LPSTR command_line;

    char exe_filename[WIN32_STATE_FILE_NAME_COUNT];
    char *one_past_last_slash_exe_filename;

    char resource_path_prefix[WIN32_STATE_FILE_NAME_COUNT];
    uint32 resource_path_prefix_length;

    char library_path_prefix[WIN32_STATE_FILE_NAME_COUNT];
    uint32 library_path_prefix_length;

    WINDOWPLACEMENT window_position; // save the last window position for fullscreen purposes
    real64 inv_performance_counter_frequency;

    RECT mouse_rect;
    RECT mouse_rect_full;

    bool32 capture_mouse;
    bool32 hide_mouse;
    bool32 is_fullscreen;

    HDC device_context;

    WORD default_console_attributes_out;
    WORD default_console_attributes_err;
    bool32 alloced_console;
};
global_variable PlatformState global_win32_state;

LRESULT CALLBACK win32_window_callback(HWND window, uint32 message, WPARAM w_param, LPARAM l_param);
internal_func void Win32GetResourcePath(char* ResourcePathPrefix, uint8 PrefixLength);

internal_func void CatStrings(size_t SourceACount, const char *SourceA,
                              size_t SourceBCount, const char *SourceB,
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

internal_func size_t StringLength(const char* String) {
    size_t Count = 0;
    while(*String++) {
        ++Count;
    }
    return Count;
}

internal_func void Win32GetResourcePath(char* ResourcePathPrefix, uint8 PrefixLength) {
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

        CatStrings(PrefixLength, ResourcePathPrefix, 0, 0, PrefixLength, global_win32_state.resource_path_prefix);
        global_win32_state.resource_path_prefix_length = PrefixLength;
    }
}
internal_func void Win32GetLibraryPath(char* LibraryPathPrefix, size_t PrefixLength) {
    if (PrefixLength) {
        // change all '\\' to '/'
        for (char* Scan = LibraryPathPrefix; *Scan; Scan++) {
            if (*Scan == '\\') {
                *Scan = '/';
            }
        }
        if (LibraryPathPrefix[PrefixLength-1] != '/') {
            LibraryPathPrefix[PrefixLength] = '/';
            PrefixLength++;
        }

        CatStrings(PrefixLength, LibraryPathPrefix, 0, 0, PrefixLength, global_win32_state.library_path_prefix);
        global_win32_state.library_path_prefix_length = (uint32)PrefixLength;
    }
}

void platform_init_logging(bool32 create_console) {
    AttachConsole((DWORD)-1);

    // setup console defaults
    if (create_console) {
        AllocConsole();
        RH_DEBUG("Creating new console");
    }
    global_win32_state.alloced_console = create_console;

    CONSOLE_SCREEN_BUFFER_INFO console_info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &console_info);
    global_win32_state.default_console_attributes_out = console_info.wAttributes;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &console_info);
    global_win32_state.default_console_attributes_err = console_info.wAttributes;

    #if RH_INTERNAL
    // move console to a nicer positon
    // note: this -seems- to only control the console that gets spawned by the app.
    //       i.e.: if you run from command line by typing game.exe for example,
    //       the console does not move. This is how i want it!
    HWND console_window = GetConsoleWindow();
    SetWindowPos(console_window, HWND_TOP, 900, 17, 1003, 1000, 0);
    #endif
}
bool32 platform_startup(AppConfig* config) {
    global_win32_state.instance = GetModuleHandleA(NULL);
    global_win32_state.command_line = GetCommandLineA();

    LARGE_INTEGER PerfCounterFrequencyResult;
    QueryPerformanceFrequency(&PerfCounterFrequencyResult);
    int64 performance_counter_frequency = PerfCounterFrequencyResult.QuadPart;
    global_win32_state.inv_performance_counter_frequency = 1.0 / ((real64)performance_counter_frequency);

    // get exe filename/path
    {
        DWORD SizeOfFilename = GetModuleFileNameA(0, global_win32_state.exe_filename, sizeof(global_win32_state.exe_filename));
        global_win32_state.one_past_last_slash_exe_filename = global_win32_state.exe_filename;
        for (char *Scan = global_win32_state.exe_filename; *Scan; ++Scan) {
            if (*Scan == '\\') {
                global_win32_state.one_past_last_slash_exe_filename = Scan + 1;
            }
        }
    }

    // get resource path prefix
    {
        bool32 FoundResourcePath = false;
        char ResourcePathPrefix[256] = { 0 };
        uint8 PrefixLength = 0;
        for (char* Scan = global_win32_state.command_line; *Scan; Scan++) {
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
            RH_DEBUG("Run with a -r flag");
            Win32GetResourcePath(ResourcePathPrefix, PrefixLength);
        } else {
            DWORD dwAttrib = GetFileAttributes("Data");

            if (dwAttrib != INVALID_FILE_ATTRIBUTES && 
                            (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
                // the executable is right next to the data folder, no resource prefix needed
                RH_DEBUG("Found the Data/ directory!");
            } else {
                // Neither is the case, assuming we are being run in the build/ directory
                dwAttrib = GetFileAttributes("../Game");
                if (dwAttrib != INVALID_FILE_ATTRIBUTES &&
                                (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
                    RH_DEBUG("Looks like you ran from the build/ directory!");
                    char DefaultResourcePath[] = "../Game/run_tree/";
                    Win32GetResourcePath(DefaultResourcePath, sizeof(DefaultResourcePath)-1);
                } else {
                    RH_FATAL("I don't know where I am!");
                    RH_FATAL("Shutting down...!");
                    Sleep(2500);
                    return false;
                }
            }
        }

        // resolve resource paths to get absolute prefix
        {
            char buffer[256];
            DWORD len = GetFullPathNameA(global_win32_state.resource_path_prefix,
                                         256, buffer, NULL);
            StringCchCopyA(global_win32_state.resource_path_prefix, len, buffer);
            global_win32_state.resource_path_prefix_length = lstrlenA(global_win32_state.resource_path_prefix);

            if (global_win32_state.resource_path_prefix[global_win32_state.resource_path_prefix_length] != '\\') {
                global_win32_state.resource_path_prefix[global_win32_state.resource_path_prefix_length] = '\\';
                global_win32_state.resource_path_prefix[++global_win32_state.resource_path_prefix_length]   = '\0';
            }
        }

        // same for library path
        {
            Win32GetLibraryPath(global_win32_state.exe_filename, 
                                global_win32_state.one_past_last_slash_exe_filename - global_win32_state.exe_filename);

            char buffer[256];
            DWORD len = GetFullPathNameA(global_win32_state.library_path_prefix,
                                         256, buffer, NULL);
            StringCchCopyA(global_win32_state.library_path_prefix, len, buffer);
            global_win32_state.library_path_prefix_length = lstrlenA(global_win32_state.library_path_prefix);

            if (global_win32_state.library_path_prefix[global_win32_state.library_path_prefix_length] != '\\') {
                global_win32_state.library_path_prefix[global_win32_state.library_path_prefix_length] = '\\';
                global_win32_state.library_path_prefix[++global_win32_state.library_path_prefix_length]   = '\0';
            }
        }
    }


    // NOTE: Set the Windows scheduler granularity to 1ms
    //       so that our Sleep can be more granular
    {
        UINT DesiredSchedulerMS = 1;
        if (!(timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR)) {
            RH_ERROR("Could not set Windows scheduler granularity to 1ms!");
            // could still run techincally? won't return false
        }
    }

    // create window class
    WNDCLASSA WindowClass = {};

    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = win32_window_callback;
    WindowClass.hInstance = global_win32_state.instance;
    WindowClass.hCursor = LoadCursorA(NULL, IDC_CROSS);
    WindowClass.hIcon = LoadIconA(global_win32_state.instance, IDI_APPLICATION);
    WindowClass.lpszClassName = "RhWindowClass";

    if (!RegisterClassA(&WindowClass)) {
        MessageBoxA(0, "Failed to register window class.", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    // In our config we specify a resolution. We need to find the 
    // window resolution that results in that client resolution.
    uint32 client_x = config->start_x;
    uint32 client_y = config->start_y;
    uint32 client_width = config->start_width;
    uint32 client_height = config->start_height;

    uint32 window_x = client_x;
    uint32 window_y = client_y;
    uint32 window_width = client_width;
    uint32 window_height = client_height;

    uint32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    uint32 window_ex_style = WS_EX_APPWINDOW;

    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;

    RECT border_rect = {};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    // move/resize window based on border dimmensions
    //window_x += border_rect.left; // we actually want to specify the window position (not client position)
    //window_y += border_rect.top;
    window_y += 7;
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    global_win32_state.window = CreateWindowExA(
        window_ex_style,
        WindowClass.lpszClassName, 
        config->application_name,
        window_style,
        window_x, window_y, window_width, window_height,
        0, 0, global_win32_state.instance, 0);

    RECT actual_rect;
    GetWindowRect(global_win32_state.window, &actual_rect);
    // weird: it says 0,0 but is not all the way in the corner...
    //        on my machine, 7 pixels away from the left (top is perfect)

    if (!global_win32_state.window) {
        MessageBoxA(0, "Failed to create window.", "Error", MB_ICONEXCLAMATION | MB_OK);
        RH_FATAL("Window creation failed!");
        return false;
    }

    global_win32_state.device_context = GetDC(global_win32_state.window);   

    ShowWindow(global_win32_state.window, SW_SHOW);

    GetClipCursor(&global_win32_state.mouse_rect_full);
    //GetWindowRect(global_win32_state.window, &client_rect);
    //GetClientRect(global_win32_state.window, &mouse_Rect);
    global_win32_state.mouse_rect.bottom = client_y + client_height;
    global_win32_state.mouse_rect.top = client_y;
    global_win32_state.mouse_rect.left = client_x;
    global_win32_state.mouse_rect.right = client_x + client_width;
    if (global_win32_state.capture_mouse) {
        ClipCursor(&global_win32_state.mouse_rect);
    }
    
    //GlobalMonitorRefreshHz = 60;
    //GlobalGameUpdateHz = 60;
    //GlobalMonitorRefreshHz = Win32GetMonitorRefreshRate(Window);

    RAWINPUTDEVICE Rid[1];
    Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
    Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE; 
    Rid[0].dwFlags = RIDEV_INPUTSINK;   
    Rid[0].hwndTarget = global_win32_state.window;
    RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

    return true;
}

void platform_shutdown() {
    if (global_win32_state.alloced_console){
        FreeConsole();
    }
    // don't really need to do anything here...
    RH_INFO("Shutting down the platform layer.");
}

bool32 platform_process_messages() {
    MSG Message;
    while (PeekMessageA(&Message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&Message);
        DispatchMessageA(&Message);
    }
    return true;
}

// allocated memory in page-size from the os.
// don't use this function for small dynamic allocations!!
void* platform_alloc(uint64 size, uint64 base_address) {
    return VirtualAlloc((LPVOID)base_address, (size_t)size, 
                        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void platform_free(void* memory) {
    VirtualFree(memory, 0, MEM_RELEASE);
}

global_variable WORD console_level_colors[6] = {64, 4, 6, 2, 1, 8};
void platform_console_write_error(const char* Message, uint8 Color) {
    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    SetConsoleTextAttribute(console_handle, console_level_colors[Color]);
    OutputDebugStringA(Message);
    uint64 length = strlen(Message);
    DWORD number_written = 0;
    WriteConsoleA(console_handle, Message, (DWORD)length, &number_written, 0);
    SetConsoleTextAttribute(console_handle, global_win32_state.default_console_attributes_err);
}

void platform_console_write(const char* Message, uint8 Color) {
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    SetConsoleTextAttribute(console_handle, console_level_colors[Color]);
    OutputDebugStringA(Message);
    uint64 length = strlen(Message);
    DWORD number_written = 0;
    WriteConsoleA(console_handle, Message, (DWORD)length, &number_written, 0);
    SetConsoleTextAttribute(console_handle, global_win32_state.default_console_attributes_out);
}

void platform_console_set_title(const char* Message, ...) {

    char title_buffer[256];
    va_list args;
    va_start(args, Message);
    vsnprintf(title_buffer, sizeof(title_buffer), Message, args);
    va_end(args);
    SetConsoleTitleA(title_buffer);
}

int64 platform_get_wall_clock() {
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);

    return Result.QuadPart;
}
real64 platform_get_seconds_elapsed(int64 start, int64 end) {
    return ((real64)(end - start)) * global_win32_state.inv_performance_counter_frequency;
}

void platform_sleep(uint64 ms) {
    Sleep((DWORD)ms);
}


bool32 win32_toggle_fullscreen(HWND Window, WINDOWPLACEMENT* WindowPos) {
    // TODO: Look into ChangeDisplaySettings function to change monitor refresh rate/resolution
    DWORD Style = GetWindowLong(Window, GWL_STYLE);
    if (Style & WS_OVERLAPPEDWINDOW) {
        MONITORINFO MonitorInfo = { sizeof(MonitorInfo) };
        if (GetWindowPlacement(Window, WindowPos) &&GetMonitorInfo(MonitorFromWindow(Window, MONITOR_DEFAULTTOPRIMARY), &MonitorInfo)) {
            SetWindowLong(Window, GWL_STYLE, Style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(Window, HWND_TOP,
                         MonitorInfo.rcMonitor.left, MonitorInfo.rcMonitor.top,
                         MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left,
                         MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
        return true;
    } else {
        SetWindowLong(Window, GWL_STYLE,
                      Style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(Window, WindowPos);
        SetWindowPos(Window, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        return false;
    }
}

void win32_update_mouse_rect(HWND window, long new_width, long new_height, RECT* mouse_rect) {
    POINT top_left = {0, 0};
    POINT bottom_right = {new_width, new_height};
    ClientToScreen(window,&top_left);
    ClientToScreen(window,&bottom_right);
    mouse_rect->top = top_left.y;
    mouse_rect->left = top_left.x;
    mouse_rect->bottom = bottom_right.y - 1; // -1 for some reason is needed
    mouse_rect->right = bottom_right.x - 1;
}

LRESULT CALLBACK win32_window_callback(HWND window, uint32 message, WPARAM w_param, LPARAM l_param) {
    if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param))
        return true;

    event_context no_data = {};
    switch (message) {
        case WM_ERASEBKGND:
            return 1;
        case WM_CLOSE:
            event_fire(EVENT_CODE_APPLICATION_QUIT, 0, no_data);
            return 0;
        case WM_DESTROY:
            return 0;
        case WM_SIZE: {
            RECT client_rect;
            GetClientRect(window, &client_rect);
            uint32 width  = client_rect.right - client_rect.left;
            uint32 height = client_rect.bottom - client_rect.top;

            event_context context;
            context.u32[0] = width;
            context.u32[1] = height;
            event_fire(EVENT_CODE_RESIZED, 0, context); // this will generate tons of resize messages, beware!
            
            win32_update_mouse_rect(window, (long)width, (long)height, &global_win32_state.mouse_rect);
            if (global_win32_state.capture_mouse) {
                ClipCursor(&global_win32_state.mouse_rect);
            }
        } break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            uint8 pressed = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN);
            keyboard_keys key = static_cast<keyboard_keys>(w_param);

            // Check for extended scan code.
            uint8 is_extended = (HIWORD(l_param) & KF_EXTENDED) == KF_EXTENDED;

            // Keypress only determines if _any_ alt/ctrl/shift key is pressed. Determine which one if so.
            if (w_param == VK_MENU) {
                key = is_extended ? KEY_RALT : KEY_LALT;
            } else if (w_param == VK_SHIFT) {
                // Annoyingly, KF_EXTENDED is not set for shift keys.
                uint32 left_shift = MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC);
                uint32 scancode = ((l_param & (0xFF << 16)) >> 16);
                key = scancode == left_shift ? KEY_LSHIFT : KEY_RSHIFT;
            } else if (w_param == VK_CONTROL) {
                key = is_extended ? KEY_RCONTROL : KEY_LCONTROL;
            }

            //RH_DEBUG("Key [%s]=[%d] %s", input_get_key_string(key), key, pressed ? "down" : "up");

            input_process_key(key, (uint8)pressed);

            bool32 AltKeyWasDown = (l_param & (1 << 29));
            if (pressed && AltKeyWasDown) {
                if (key == KEY_RETURN) {
                    RH_INFO("Toggle Fullscreen");
                    global_win32_state.is_fullscreen = win32_toggle_fullscreen(global_win32_state.window, &global_win32_state.window_position);
                } else if (key == KEY_M) {
                    RH_INFO("Toggle mouse capture");
                    global_win32_state.capture_mouse = !global_win32_state.capture_mouse;
                    if (global_win32_state.capture_mouse) {
                        ClipCursor(&global_win32_state.mouse_rect);
                    } else {
                        ClipCursor(&global_win32_state.mouse_rect_full);
                    }
                } else if (key == KEY_H) {
                    RH_INFO("Toggle mouse hide");
                    global_win32_state.hide_mouse = !global_win32_state.hide_mouse;
                    if (global_win32_state.hide_mouse) {
                        ShowCursor(FALSE);
                    } else {
                        ShowCursor(TRUE);
                    }
                }
            }

            return 0;
        } break;
        case WM_MOUSEMOVE: {
            int32 x_pos = GET_X_LPARAM(l_param);
            int32 y_pos = GET_Y_LPARAM(l_param);

            input_process_mouse_move(x_pos, y_pos);
        } break;
        case WM_INPUT: 
        {
            UINT dwSize = sizeof(RAWINPUT);
            static BYTE lpb[sizeof(RAWINPUT)];

            GetRawInputData((HRAWINPUT)l_param, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

            RAWINPUT* raw = (RAWINPUT*)lpb;

            if (raw->header.dwType == RIM_TYPEMOUSE) 
            {
                int xPosRelative = raw->data.mouse.lLastX;
                int yPosRelative = raw->data.mouse.lLastY;

                //RH_TRACE("Mouse dx: %d", xPosRelative);
                input_process_raw_mouse_move(xPosRelative, yPosRelative);
            } 
            break;
        } break;
        case WM_MOUSEWHEEL: {
            int32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            if (z_delta != 0) {
                z_delta = (z_delta < 0) ? -1 : 1;
            }

            input_process_mouse_wheel(z_delta);
        }
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            uint8 pressed = (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN);
            mouse_button_codes mouse_button = BUTTON_MAX_BUTTONS;
            switch (message) {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                    mouse_button = BUTTON_LEFT;
                    break;
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                    mouse_button = BUTTON_MIDDLE;
                    break;
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                    mouse_button = BUTTON_RIGHT;
                    break;
            }

            if (mouse_button != BUTTON_MAX_BUTTONS) {
                input_process_mouse_button(mouse_button, pressed);
            }
        } break;
    }

    return DefWindowProcA(window, message, w_param, l_param);
}

int win32_get_monitor_refresh_rate() {
    int MonitorRefreshHz = 0;
    HMONITOR CurrentMonitor = MonitorFromWindow(global_win32_state.window, MONITOR_DEFAULTTONEAREST);
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

void platform_swap_buffers() {
    SwapBuffers(global_win32_state.device_context);
}


// Memory utils
void* memory_zero(void* memory, uint64 size) {
    return SecureZeroMemory(memory, size);
}

void* memory_copy(void* dest, const void* src, uint64 size) {
    CopyMemory(dest, src, size);
    return dest;
}

void* memory_set(void* memory, uint8 value, uint64 size) {
    FillMemory(memory, size, value);
    return memory;
}

// file IO
size_t platform_get_full_resource_path(char* buffer, size_t buffer_length, const char* resource_path) {
    Assert(global_win32_state.resource_path_prefix);

    size_t resource_path_length = StringLength(resource_path);

    Assert(resource_path_length + global_win32_state.resource_path_prefix_length < buffer_length);
    CatStrings(global_win32_state.resource_path_prefix_length, global_win32_state.resource_path_prefix,
               resource_path_length, resource_path,
               buffer_length, buffer);

    size_t full_length = 0;
    for (char* scan = buffer; *scan; scan++) {
        full_length++;
        if (*scan == '/') {
            *scan = '\\';
        }
    }
    return full_length;
}
size_t platform_get_full_library_path(char* buffer, size_t buffer_length, const char* library_path) {
    Assert(global_win32_state.library_path_prefix);

    size_t resource_path_length = StringLength(library_path);

    Assert(resource_path_length + global_win32_state.library_path_prefix_length < buffer_length);
    CatStrings(global_win32_state.library_path_prefix_length, global_win32_state.library_path_prefix,
               resource_path_length, library_path,
               buffer_length, buffer);

    size_t full_length = 0;
    for (char* scan = buffer; *scan; scan++) {
        full_length++;
        if (*scan == '/') {
            *scan = '\\';
        }
    }
    return full_length;
}

file_handle platform_read_entire_file(const char* full_path) {
    file_handle file = {};

    HANDLE FileHandle = CreateFileA(full_path, 
                                    GENERIC_READ, FILE_SHARE_READ, NULL, 
                                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE != FileHandle) {
        LARGE_INTEGER FileSizeBytes;
        if (GetFileSizeEx(FileHandle, &FileSizeBytes)) {

            FileSizeBytes.QuadPart++; // +1 for null-terminator!
            LPVOID Buffer = VirtualAlloc(0, FileSizeBytes.QuadPart,
                                         MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

            DWORD BytesRead;
            if (Buffer) {
                if (ReadFile(FileHandle, Buffer, (DWORD)FileSizeBytes.QuadPart, &BytesRead, NULL)) {
                    Assert((BytesRead+1) == FileSizeBytes.QuadPart);// +1 for null-terminator!

                    file.num_bytes = BytesRead;
                    file.data = (uint8*)Buffer;
                    file.data[BytesRead] = 0; // add a null-terminator just in case!

                    CloseHandle(FileHandle);

                    //RH_TRACE("File: %s\n"
                    //         "               %llu Bytes", full_path, file.num_bytes);

                    return file;
                }
                VirtualFree(Buffer, 0, MEM_RELEASE);
            }
        }

    }

    return file;
}
void platform_free_file_data(file_handle* handle) {
    Assert(handle);
    if (handle->num_bytes > 0 && handle->data) {
        VirtualFree(handle->data, 0, MEM_RELEASE);
    }

    handle->num_bytes = 0;
    handle->data = 0;
}

//struct file_info {
//    uint64 file_attributes;
//    uint64 creation_time;
//    uint64 last_access_time;
//    uint64 last_write_time;
//    uint64 file_size;
//};
uint64 uint64_from_DWORDS(DWORD low, DWORD high) {
    uint64 L = (uint64)low;
    uint64 H = (uint64)high;
    return (H<<32) | L;
}
void DWORDS_from_uint64(uint64 val, DWORD* low, DWORD* high) {
    uint64 L = val & 0x00000000FFFFFFFF; // lower 32 bits
    uint64 H = val & 0xFFFFFFFF00000000; // upper 32 bits

    *low  = (DWORD)L;
    *high = (DWORD)(H >> 32);
}
bool32 platform_get_file_attributes(const char* full_path, file_info* info) {
    WIN32_FILE_ATTRIBUTE_DATA Data;
    if (GetFileAttributesExA(full_path, GetFileExInfoStandard, &Data)) {
        DWORD FileAttributes    = Data.dwFileAttributes;
        FILETIME CreationTime   = Data.ftCreationTime;
        FILETIME LastAccessTime = Data.ftLastAccessTime;
        FILETIME LastWriteTime  = Data.ftLastWriteTime;
        DWORD FilesizeHigh      = Data.nFileSizeHigh;
        DWORD FilesizeLow       = Data.nFileSizeLow;

        info->file_attributes = (uint64)FileAttributes;
        info->creation_time    = uint64_from_DWORDS(CreationTime.dwLowDateTime,   CreationTime.dwHighDateTime);
        info->last_access_time = uint64_from_DWORDS(LastAccessTime.dwLowDateTime, LastAccessTime.dwHighDateTime);
        info->last_write_time  = uint64_from_DWORDS(LastWriteTime.dwLowDateTime,  LastWriteTime.dwHighDateTime);
        info->file_size        = uint64_from_DWORDS(FilesizeLow,                  FilesizeHigh);
        return true;
    }

    //RH_TRACE("Could not get file-info for '%s'", full_path);
    return false;
}

void platform_filetime_to_systime(uint64 file_time, char* buffer, uint64 buf_size) {
    // convert to cst
    const uint64 one_hour = 36000000000; // 1 hour in 100-nanosecond intervals
    file_time -= (5 * one_hour); // cst is UTC-5

    FILETIME ft;
    DWORDS_from_uint64(file_time, &ft.dwLowDateTime, &ft.dwHighDateTime);

    SYSTEMTIME sys_time;
    FileTimeToSystemTime(&ft, &sys_time);

    WORD hour = sys_time.wHour;
    char* am_pm = "am";
    if (hour >= 12) {
        hour -= 12; // get rid of 24 hour time
        am_pm = "pm";
    }
    if (hour == 0)  hour  = 12; // turn hour 0 to 12:00


    wnsprintfA(buffer, (int)buf_size,
              "%02hu/%02hu/%04hu %02hu:%02hu:%02hu.%03hu %s",
              sys_time.wMonth, sys_time.wDay,    sys_time.wYear,
              hour,  sys_time.wMinute, sys_time.wSecond, sys_time.wMilliseconds, am_pm);
}

bool32 platform_copy_file(const char* src_path, const char* dst_path) {
    if (CopyFileA(src_path, dst_path, FALSE) != 0) return true;

    DWORD code = GetLastError();
    RH_ERROR("Could not copy file from '%s' to '%s'. Error:[%u]", src_path, dst_path, code);
    return false;
}

void* platform_load_shared_library(const char* lib_path) {
    return LoadLibraryA(lib_path);
}
void* platform_get_func_from_lib(void* shared_lib, const char* func_name) {
    return GetProcAddress((HMODULE)shared_lib, func_name);
}
void platform_unload_shared_library(void* shared_lib) {
    FreeLibrary((HMODULE)shared_lib);
}

void platform_update_mouse() {
    if (global_win32_state.hide_mouse) {
        LONG center_x = (global_win32_state.mouse_rect.left + global_win32_state.mouse_rect.right) / 2;
        LONG center_y = (global_win32_state.mouse_rect.bottom + global_win32_state.mouse_rect.top) / 2;

        SetCursorPos(center_x,center_y);
    }
}

#endif //#if RH_PLATFORM_WINDOWS