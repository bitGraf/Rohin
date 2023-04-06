#include "Platform.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"
#include "Engine/Core/Event.h"
#include "Engine/Memory/MemoryUtils.h"
#include "Engine/Core/Input.h"


#ifdef RH_PLATFORM_LINUX
#include <cstdio>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>  // sudo apt-get install libx11-dev
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>  // sudo apt-get install libxkbcommon-x11-dev libx11-xcb-dev

#include <sys/time.h>
#if _POSIX_C_SOURCE >= 199309L
#include <time.h>  // nanosleep
#endif

// platform_linux.cpp manages this struct, no one else needs to access it
typedef struct linux_handle_info {
    xcb_connection_t* connection;
    xcb_window_t window;
} linux_handle_info;
struct PlatformState {
    Display* display;
    linux_handle_info handle;
    xcb_screen_t* screen;
    xcb_atom_t wm_protocols;
    xcb_atom_t wm_delete_win;



    //HINSTANCE instance;
    //HWND window;
    char* command_line;

    //char exe_filename[WIN32_STATE_FILE_NAME_COUNT];
    //char *one_past_last_slash_exe_filename;

    //char resource_path_prefix[WIN32_STATE_FILE_NAME_COUNT];
    //uint32 resource_path_prefix_length;

    //WINDOWPLACEMENT window_position; // save the last window position for fullscreen purposes

    //RECT mouse_rect;
    //RECT mouse_rect_full;

    bool32 capture_mouse;
    bool32 hide_mouse;
    bool32 is_fullscreen;

    //HDC device_context;
};
global_variable PlatformState global_linux_state;

// Key translation from X11 to windows defines
keyboard_keys translate_keycode(uint32 x_keycode);

bool32 platform_startup(AppConfig* config) { 
    #pragma message ( "NOT FULLY IMPLEMENTED FOR LINUX" )

    // connect to X
    global_linux_state.display = XOpenDisplay(NULL);

    XAutoRepeatOff(global_linux_state.display);

    global_linux_state.handle.connection = XGetXCBConnection(global_linux_state.display);
    if (xcb_connection_has_error(global_linux_state.handle.connection)) {
        RH_FATAL("Failed to connect to X server via XCB.");
        return false;
    }

    // Get data from the X server
    const xcb_setup_t *setup = xcb_get_setup(global_linux_state.handle.connection);

    // Loop through screens using iterator
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    int screen_p = 0;
    for (int32 s = screen_p; s > 0; s--) {
        xcb_screen_next(&it);
    }

    // After screens have been looped through, assign it.
    global_linux_state.screen = it.data;

    // Allocate a XID for the window to be created.
    global_linux_state.handle.window = xcb_generate_id(global_linux_state.handle.connection);

    // Register event types.
    // XCB_CW_BACK_PIXEL = filling then window bg with a single colour
    // XCB_CW_EVENT_MASK is required.
    uint32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    // Listen for keyboard and mouse buttons
    uint32 event_values = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                          XCB_EVENT_MASK_KEY_PRESS    | XCB_EVENT_MASK_KEY_RELEASE    |
                          XCB_EVENT_MASK_EXPOSURE     | XCB_EVENT_MASK_POINTER_MOTION |
                          XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    // Values to be sent over XCB (bg colour, events)
    uint32 value_list[] = {global_linux_state.screen->black_pixel, event_values};

    // Create the window
    xcb_create_window(
        global_linux_state.handle.connection,
        XCB_COPY_FROM_PARENT,                   // depth
        global_linux_state.handle.window,
        global_linux_state.screen->root,        // parent
        config->start_x,                              // x
        config->start_y,                              // y
        config->start_width,                          // width
        config->start_height,                         // height
        0,                                      // No border
        XCB_WINDOW_CLASS_INPUT_OUTPUT,          // class
        global_linux_state.screen->root_visual,
        event_mask,
        value_list);

    // Change the title
    xcb_change_property(
        global_linux_state.handle.connection,
        XCB_PROP_MODE_REPLACE,
        global_linux_state.handle.window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,  // data should be viewed 8 bits at a time
        strlen(config->application_name),
        config->application_name);

    // Tell the server to notify when the window manager
    // attempts to destroy the window.
    xcb_intern_atom_cookie_t wm_delete_cookie = xcb_intern_atom(
        global_linux_state.handle.connection,
        0,
        strlen("WM_DELETE_WINDOW"),
        "WM_DELETE_WINDOW");
    xcb_intern_atom_cookie_t wm_protocols_cookie = xcb_intern_atom(
        global_linux_state.handle.connection,
        0,
        strlen("WM_PROTOCOLS"),
        "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* wm_delete_reply = xcb_intern_atom_reply(
        global_linux_state.handle.connection,
        wm_delete_cookie,
        NULL);
    xcb_intern_atom_reply_t* wm_protocols_reply = xcb_intern_atom_reply(
        global_linux_state.handle.connection,
        wm_protocols_cookie,
        NULL);
    global_linux_state.wm_delete_win = wm_delete_reply->atom;
    global_linux_state.wm_protocols = wm_protocols_reply->atom;

    xcb_change_property(
        global_linux_state.handle.connection,
        XCB_PROP_MODE_REPLACE,
        global_linux_state.handle.window,
        wm_protocols_reply->atom,
        4,
        32,
        1,
        &wm_delete_reply->atom);

    // Map the window to the screen
    xcb_map_window(global_linux_state.handle.connection, global_linux_state.handle.window);

    // Flush the stream
    int32 stream_result = xcb_flush(global_linux_state.handle.connection);
    if (stream_result <= 0) {
        RH_FATAL("An error occurred when flusing the stream: %d", stream_result);
        return false;
    }

    return true;
}
void platform_shutdown() {
    // Turn key repeats back on since this is global for the OS... just... wow.
    XAutoRepeatOn(global_linux_state.display);

    xcb_destroy_window(global_linux_state.handle.connection, global_linux_state.handle.window);

    RH_INFO("Shutting down platform.");
}
bool32 platform_process_messages() { 
    xcb_generic_event_t* event;
    xcb_client_message_event_t* cm;

    bool32 quit_flagged = false;

    // Poll for events until null is returned.
    while ((event = xcb_poll_for_event(global_linux_state.handle.connection))) {
        // Input events
        switch (event->response_type & ~0x80) {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE: {
                // Key press event - xcb_key_press_event_t and xcb_key_release_event_t are the same
                xcb_key_press_event_t* kb_event = (xcb_key_press_event_t*)event;
                bool32 pressed = event->response_type == XCB_KEY_PRESS;
                xcb_keycode_t code = kb_event->detail;
                KeySym key_sym = XkbKeycodeToKeysym(
                    global_linux_state.display,
                    (KeyCode)code,  // event.xkey.keycode,
                    0,
                    0 /*code & ShiftMask ? 1 : 0*/);

                keyboard_keys key = translate_keycode(key_sym);

                // Pass to the input subsystem for processing.
                input_process_key(key, pressed);
            } break;
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE: {
                xcb_button_press_event_t* mouse_event = (xcb_button_press_event_t*)event;
                bool32 pressed = event->response_type == XCB_BUTTON_PRESS;
                mouse_button_codes mouse_button = BUTTON_MAX_BUTTONS;
                switch (mouse_event->detail) {
                    case XCB_BUTTON_INDEX_1:
                        mouse_button = BUTTON_LEFT;
                        break;
                    case XCB_BUTTON_INDEX_2:
                        mouse_button = BUTTON_MIDDLE;
                        break;
                    case XCB_BUTTON_INDEX_3:
                        mouse_button = BUTTON_RIGHT;
                        break;
                }

                // Pass over to the input subsystem.
                if (mouse_button != BUTTON_MAX_BUTTONS) {
                    input_process_mouse_button(mouse_button, pressed);
                }
            } break;
            case XCB_MOTION_NOTIFY: {
                // Mouse move
                xcb_motion_notify_event_t* move_event = (xcb_motion_notify_event_t*)event;

                // Pass over to the input subsystem.
                input_process_mouse_move(move_event->event_x, move_event->event_y);
            } break;
            case XCB_CONFIGURE_NOTIFY: {
                // Resizing - note that this is also triggered by moving the window, but should be
                // passed anyway since a change in the x/y could mean an upper-left resize.
                // The application layer can decide what to do with this.
                xcb_configure_notify_event_t* configure_event = (xcb_configure_notify_event_t*)event;

                // Fire the event. The application layer should pick this up, but not handle it
                // as it shouldn be visible to other parts of the application.
                event_context context;
                context.u16[0] = configure_event->width;
                context.u16[1] = configure_event->height;
                event_fire(EVENT_CODE_RESIZED, 0, context);

            } break;

            case XCB_CLIENT_MESSAGE: {
                cm = (xcb_client_message_event_t*)event;

                // Window close
                if (cm->data.data32[0] == global_linux_state.wm_delete_win) {
                    quit_flagged = true;
                }
            } break;
            default:
                // Something else
                break;
        }

        free(event);
    }

    //// Update watches.
    //platform_update_watches();

    return !quit_flagged;
}


void* platform_alloc(uint64 size, uint64 base_address) {
    RH_WARN("!!base_address not supported on this platform!!");
    return malloc(size);
//    void* mem = mmap((void*)base_address, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
//
//    if (mem == MAP_FAILED) {
//        RH_FATAL("Failed to alloc!");
//    }
//    
//    return mem;
}
void platform_free(void* memory) {
    free(memory);
    // munmap(memory, size);
}

void platform_console_write(const char* Message, uint8 color) {
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    const char* colour_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", colour_strings[color], Message);
}
void platform_console_write_error(const char* Message, uint8 color) {
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    const char* colour_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", colour_strings[color], Message);
}

void platform_console_set_title(const char* Message, ...) {
    //#pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
    printf("\033]0;%s\007", Message);
}
int64 platform_get_wall_clock() { 
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);

    // Note: this is handled differently on linux, so this is done to match
    //       the usage on Windows. This function will return 'nanoseconds',
    //       and platform_get_seconds_elapsed() will calculate seconds
    //       elapsed from that

    return now.tv_sec * 1000000000 + now.tv_nsec;
    //return now.tv_sec + now.tv_nsec * 0.000000001;
}
real64 platform_get_seconds_elapsed(int64 start, int64 end) { 
    // Note: start and end are times in 'nanoseconds'.
    //       return value is delta-time in 'seconds'

    return (end - start) * 0.000000001;
}
void platform_sleep(uint64 ms) {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000 * 1000;
    nanosleep(&ts, 0);
#else
    if (ms >= 1000) {
        sleep(ms / 1000);
    }
    usleep((ms % 1000) * 1000);
#endif
}

void platform_update_mouse() {
    if (global_linux_state.hide_mouse) {
        int screen = DefaultScreen(global_linux_state.display);
        unsigned int width  = DisplayWidth(global_linux_state.display, screen);
        unsigned int height = DisplayHeight(global_linux_state.display, screen);

        XWarpPointer(global_linux_state.display, None, global_linux_state.handle.window, 0, 0, 0, 0, width/2, height/2);
    }
}

// rendering stuff
void platform_swap_buffers() {
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
}

// file i/o
size_t platform_get_full_resource_path(char* buffer, size_t buffer_length, const char* resource_path) { 
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
    return 0;
}
file_handle platform_read_entire_file(const char* full_path) { 
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
    return {};
}
void platform_free_file_data(file_handle* handle) {
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
}

// memory
void* memory_zero(void* memory, uint64 size) { 
    return memory_set(memory, 0, size);
}
void* memory_copy(void* dest, const void* src, uint64 size) {
    return memcpy(dest, src, size);
}
void* memory_set(void* memory, uint8 value, uint64 size) { 
    return memset(memory, value, size);
}

// Key translation
keyboard_keys translate_keycode(uint32 x_keycode) {
    switch (x_keycode) {
        case XK_BackSpace:
            return KEY_BACKSPACE;
        case XK_Return:
            return KEY_RETURN;
        case XK_Tab:
            return KEY_TAB;
            // case XK_Shift: return KEY_SHIFT;
            // case XK_Control: return KEY_CONTROL;

        case XK_Pause:
            return KEY_PAUSE;
        case XK_Caps_Lock:
            return KEY_CAPSLOCK;

        case XK_Escape:
            return KEY_ESCAPE;

            // Not supported
            // case : return KEY_CONVERT;
            // case : return KEY_NONCONVERT;
            // case : return KEY_ACCEPT;

        //case XK_Mode_switch:
        //    return KEY_MODECHANGE;

        case XK_space:
            return KEY_SPACE;
        case XK_Prior:
            return KEY_PAGEUP;
        case XK_Next:
            return KEY_PAGEDN;
        case XK_End:
            return KEY_END;
        case XK_Home:
            return KEY_HOME;
        case XK_Left:
            return KEY_LEFT;
        case XK_Up:
            return KEY_UP;
        case XK_Right:
            return KEY_RIGHT;
        case XK_Down:
            return KEY_DOWN;
        //case XK_Select:
        //    return KEY_SELECT;
        case XK_Print:
            return KEY_PRINTSCR;
        //case XK_Execute:
        //    return KEY_EXECUTE;
        // case XK_snapshot: return KEY_SNAPSHOT; // not supported
        case XK_Insert:
            return KEY_INSERT;
        case XK_Delete:
            return KEY_DELETE;
        //case XK_Help:
        //    return KEY_HELP;

        //case XK_Meta_L:
        //    return KEY_LSUPER;  // TODO: not sure this is right
        //case XK_Meta_R:
        //    return KEY_RSUPER;
            // case XK_apps: return KEY_APPS; // not supported

            // case XK_sleep: return KEY_SLEEP; //not supported

        case XK_KP_0:
            return KEY_NUMPAD0;
        case XK_KP_1:
            return KEY_NUMPAD1;
        case XK_KP_2:
            return KEY_NUMPAD2;
        case XK_KP_3:
            return KEY_NUMPAD3;
        case XK_KP_4:
            return KEY_NUMPAD4;
        case XK_KP_5:
            return KEY_NUMPAD5;
        case XK_KP_6:
            return KEY_NUMPAD6;
        case XK_KP_7:
            return KEY_NUMPAD7;
        case XK_KP_8:
            return KEY_NUMPAD8;
        case XK_KP_9:
            return KEY_NUMPAD9;
        case XK_multiply:
            return KEY_MULTIPLY;
        case XK_KP_Add:
            return KEY_ADD;
        case XK_KP_Separator:
            return KEY_SEPARATOR;
        case XK_KP_Subtract:
            return KEY_SUBTRACT;
        case XK_KP_Decimal:
            return KEY_DECIMAL;
        case XK_KP_Divide:
            return KEY_DIVIDE;
        case XK_F1:
            return KEY_F1;
        case XK_F2:
            return KEY_F2;
        case XK_F3:
            return KEY_F3;
        case XK_F4:
            return KEY_F4;
        case XK_F5:
            return KEY_F5;
        case XK_F6:
            return KEY_F6;
        case XK_F7:
            return KEY_F7;
        case XK_F8:
            return KEY_F8;
        case XK_F9:
            return KEY_F9;
        case XK_F10:
            return KEY_F10;
        case XK_F11:
            return KEY_F11;
        case XK_F12:
            return KEY_F12;
        //case XK_F13:
        //    return KEY_F13;
        //case XK_F14:
        //    return KEY_F14;
        //case XK_F15:
        //    return KEY_F15;
        //case XK_F16:
        //    return KEY_F16;
        //case XK_F17:
        //    return KEY_F17;
        //case XK_F18:
        //    return KEY_F18;
        //case XK_F19:
        //    return KEY_F19;
        //case XK_F20:
        //    return KEY_F20;
        //case XK_F21:
        //    return KEY_F21;
        //case XK_F22:
        //    return KEY_F22;
        //case XK_F23:
        //    return KEY_F23;
        //case XK_F24:
        //    return KEY_F24;

        case XK_Num_Lock:
            return KEY_NUMLOCK;
        case XK_Scroll_Lock:
            return KEY_SCRLOCK;

        //case XK_KP_Equal:
        //    return KEY_NUMPAD_EQUAL;

        case XK_Shift_L:
            return KEY_LSHIFT;
        case XK_Shift_R:
            return KEY_RSHIFT;
        case XK_Control_L:
            return KEY_LCONTROL;
        case XK_Control_R:
            return KEY_RCONTROL;
        case XK_Alt_L:
            return KEY_LALT;
        case XK_Alt_R:
            return KEY_RALT;

        //case XK_semicolon:
        //    return KEY_SEMICOLON;
        //case XK_plus:
        //    return KEY_EQUAL;
        //case XK_comma:
        //    return KEY_COMMA;
        //case XK_minus:
        //    return KEY_MINUS;
        //case XK_period:
        //    return KEY_PERIOD;
        //case XK_slash:
        //    return KEY_SLASH;
        //case XK_grave:
        //    return KEY_GRAVE;

        case XK_0:
            return KEY_0;
        case XK_1:
            return KEY_1;
        case XK_2:
            return KEY_2;
        case XK_3:
            return KEY_3;
        case XK_4:
            return KEY_4;
        case XK_5:
            return KEY_5;
        case XK_6:
            return KEY_6;
        case XK_7:
            return KEY_7;
        case XK_8:
            return KEY_8;
        case XK_9:
            return KEY_9;

        case XK_a:
        case XK_A:
            return KEY_A;
        case XK_b:
        case XK_B:
            return KEY_B;
        case XK_c:
        case XK_C:
            return KEY_C;
        case XK_d:
        case XK_D:
            return KEY_D;
        case XK_e:
        case XK_E:
            return KEY_E;
        case XK_f:
        case XK_F:
            return KEY_F;
        case XK_g:
        case XK_G:
            return KEY_G;
        case XK_h:
        case XK_H:
            return KEY_H;
        case XK_i:
        case XK_I:
            return KEY_I;
        case XK_j:
        case XK_J:
            return KEY_J;
        case XK_k:
        case XK_K:
            return KEY_K;
        case XK_l:
        case XK_L:
            return KEY_L;
        case XK_m:
        case XK_M:
            return KEY_M;
        case XK_n:
        case XK_N:
            return KEY_N;
        case XK_o:
        case XK_O:
            return KEY_O;
        case XK_p:
        case XK_P:
            return KEY_P;
        case XK_q:
        case XK_Q:
            return KEY_Q;
        case XK_r:
        case XK_R:
            return KEY_R;
        case XK_s:
        case XK_S:
            return KEY_S;
        case XK_t:
        case XK_T:
            return KEY_T;
        case XK_u:
        case XK_U:
            return KEY_U;
        case XK_v:
        case XK_V:
            return KEY_V;
        case XK_w:
        case XK_W:
            return KEY_W;
        case XK_x:
        case XK_X:
            return KEY_X;
        case XK_y:
        case XK_Y:
            return KEY_Y;
        case XK_z:
        case XK_Z:
            return KEY_Z;

        default:
            return KEY_UNKNOWN;
    }
}


void* platform_get_raw_handle() {
    return (void*)global_linux_state.display;
}

uint32 platform_get_window_id() {
    return global_linux_state.handle.window;
}

#endif