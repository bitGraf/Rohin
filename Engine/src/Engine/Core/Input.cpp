#include "Input.h"

#include "Engine/Core/Asserts.h"
#include "Engine/Core/Logger.h"
#include "Engine/Memory/MemoryUtils.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Core/Event.h"

#include "Engine/Platform/Platform.h"

// snapshot of a current keyboard state
struct keyboard_state {
    uint8 keys[KEY_MAX_KEYS];
};

// snapshot of current mouse state
struct mouse_state {
    int32 x_pos;
    int32 y_pos;
    uint8 buttons[BUTTON_MAX_BUTTONS];
};

struct input_system_state {
    keyboard_state keyboard_current;
    keyboard_state keyboard_previous;
    mouse_state mouse_current;
    mouse_state mouse_previous;

    int32 mouse_raw_dx;
    int32 mouse_raw_dy;
};

global_variable input_system_state* global_input_state;

bool32 input_init(struct memory_arena* arena) {
    global_input_state = PushStruct(arena, input_system_state);
    Assert(global_input_state);

    for (uint16 n = 0; n < KEY_MAX_KEYS; n++) {
        global_input_state->keyboard_current.keys[n] = 0;
        global_input_state->keyboard_previous.keys[n] = 0;
    }

    for (uint16 n = 0; n < BUTTON_MAX_BUTTONS; n++) {
        global_input_state->mouse_current.buttons[n] = 0;
        global_input_state->mouse_previous.buttons[n] = 0;
    }

    return true;
}
void input_shutdown() {
    global_input_state = nullptr;
}

void input_update(real32 delta_time) {
    Assert(global_input_state);

    platform_update_mouse();

    // swap previous and current input states
    // actually perform a copy, so that state is persisted
    memory_copy(&global_input_state->keyboard_previous, &global_input_state->keyboard_current, sizeof(keyboard_state));
    memory_copy(&global_input_state->mouse_previous, &global_input_state->mouse_current, sizeof(mouse_state));

    global_input_state->mouse_raw_dx = 0;
    global_input_state->mouse_raw_dy = 0;

    // TODO: move mouse cursor to center of screen if captured
}

// internal functions to respond to key events
void input_process_key(keyboard_keys key, uint8 pressed) {
    Assert(global_input_state);

    // only if the state has changed since last call/update
    if (global_input_state->keyboard_current.keys[key] != pressed) {
        global_input_state->keyboard_current.keys[key] = pressed;

        event_context data;
        data.u16[0] = (uint16)key;
        event_fire((uint16)(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED), 0, data);
    }
}

void input_process_mouse_button(mouse_button_codes button, uint8 pressed) {
    Assert(global_input_state);

    // only if the state has changed since last call/update
    if (global_input_state->mouse_current.buttons[button] != pressed) {
        global_input_state->mouse_current.buttons[button] = pressed;

        event_context data;
        data.u16[0] = (uint16)button;
        event_fire((uint16)(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED), 0, data);
    }
}
void input_process_mouse_move(int32 mouse_x, int32 mouse_y) {
    Assert(global_input_state);

    // only if the state has changed since last call/update
    if (global_input_state->mouse_current.x_pos != mouse_x || 
        global_input_state->mouse_current.y_pos != mouse_y) {

        global_input_state->mouse_current.x_pos = mouse_x;
        global_input_state->mouse_current.y_pos = mouse_y;

        //RH_TRACE("Mouse x: %d", mouse_x);

        event_context data;
        data.i32[0] = mouse_x;
        data.i32[1] = mouse_y;
        event_fire(EVENT_CODE_MOUSE_MOVED, 0, data);
    }
}
void input_process_raw_mouse_move(int32 mouse_dx, int32 mouse_dy) {
    Assert(global_input_state);

    global_input_state->mouse_raw_dx += mouse_dx;
    global_input_state->mouse_raw_dy += mouse_dy;
}

void input_process_mouse_wheel(int32 mouse_z) {
    Assert(global_input_state);

    event_context data;
    data.i32[0] = mouse_z;
    event_fire(EVENT_CODE_MOUSE_WHEEL, 0, data);
}

// ask for the state of keys
// Key down is true!
// Up is false!
RHAPI bool32 input_is_key_down(keyboard_keys key) {
    Assert(global_input_state);
    return (bool)global_input_state->keyboard_current.keys[key] == true;
}
RHAPI bool32 input_is_key_up(keyboard_keys key) {
    Assert(global_input_state);
    return (bool)global_input_state->keyboard_current.keys[key] == false;
}
RHAPI bool32 input_was_key_down(keyboard_keys key) {
    Assert(global_input_state);
    return (bool)global_input_state->keyboard_previous.keys[key] == true;
}
RHAPI bool32 input_was_key_up(keyboard_keys key) {
    Assert(global_input_state);
    return (bool)global_input_state->keyboard_previous.keys[key] == false;
}

// ask for state of mouse
// Key down is true!
// Up is false!
RHAPI bool32 input_is_button_down(mouse_button_codes button) {
    Assert(global_input_state);
    return (bool)global_input_state->mouse_current.buttons[button] == true;
}
RHAPI bool32 input_is_button_up(mouse_button_codes button) {
    Assert(global_input_state);
    return (bool)global_input_state->mouse_current.buttons[button] == false;
}
RHAPI bool32 input_was_button_down(mouse_button_codes button) {
    Assert(global_input_state);
    return (bool)global_input_state->mouse_previous.buttons[button] == true;
}
RHAPI bool32 input_was_button_up(mouse_button_codes button) {
    Assert(global_input_state);
    return (bool)global_input_state->mouse_previous.buttons[button] == false;
}

void input_get_mouse_pos(int32* x, int32* y) {
    Assert(global_input_state);
    *x = global_input_state->mouse_current.x_pos;
    *y = global_input_state->mouse_current.y_pos;
}
void input_get_prev_mouse_pos(int32* x, int32* y) {
    Assert(global_input_state);
    *x = global_input_state->mouse_previous.x_pos;
    *y = global_input_state->mouse_previous.y_pos;
}
void input_get_raw_mouse_offset(int32* dx, int32* dy) {
    Assert(global_input_state);
    *dx = global_input_state->mouse_raw_dx;
    *dy = global_input_state->mouse_raw_dy;
}

RHAPI const char* input_get_key_string(keyboard_keys key) {
    // for debug purposes
    switch (key) {
        case KEY_BACKSPACE:
            return "backspace";
        case KEY_RETURN:
            return "enter";
        case KEY_TAB:
            return "tab";
        case KEY_SHIFT:
            return "shift";
        case KEY_CONTROL:
            return "ctrl";
        case KEY_PAUSE:
            return "pause";
        case KEY_CAPSLOCK:
            return "capslock";
        case KEY_ESCAPE:
            return "esc";

        case KEY_SPACE:
            return "space";
        case KEY_PAGEUP:
            return "pageup";
        case KEY_PAGEDN:
            return "pagedown";
        case KEY_END:
            return "end";
        case KEY_HOME:
            return "home";
        case KEY_LEFT:
            return "left";
        case KEY_UP:
            return "up";
        case KEY_RIGHT:
            return "right";
        case KEY_DOWN:
            return "down";
        case KEY_PRINTSCR:
            return "printscreen";
        case KEY_INSERT:
            return "insert";
        case KEY_DELETE:
            return "delete";

        case KEY_0:
            return "0";
        case KEY_1:
            return "1";
        case KEY_2:
            return "2";
        case KEY_3:
            return "3";
        case KEY_4:
            return "4";
        case KEY_5:
            return "5";
        case KEY_6:
            return "6";
        case KEY_7:
            return "7";
        case KEY_8:
            return "8";
        case KEY_9:
            return "9";

        case KEY_A:
            return "a";
        case KEY_B:
            return "b";
        case KEY_C:
            return "c";
        case KEY_D:
            return "d";
        case KEY_E:
            return "e";
        case KEY_F:
            return "f";
        case KEY_G:
            return "g";
        case KEY_H:
            return "h";
        case KEY_I:
            return "i";
        case KEY_J:
            return "j";
        case KEY_K:
            return "k";
        case KEY_L:
            return "l";
        case KEY_M:
            return "m";
        case KEY_N:
            return "n";
        case KEY_O:
            return "o";
        case KEY_P:
            return "p";
        case KEY_Q:
            return "q";
        case KEY_R:
            return "r";
        case KEY_S:
            return "s";
        case KEY_T:
            return "t";
        case KEY_U:
            return "u";
        case KEY_V:
            return "v";
        case KEY_W:
            return "w";
        case KEY_X:
            return "x";
        case KEY_Y:
            return "y";
        case KEY_Z:
            return "z";

        // Numberpad keys
        case KEY_NUMPAD0:
            return "numpad_0";
        case KEY_NUMPAD1:
            return "numpad_1";
        case KEY_NUMPAD2:
            return "numpad_2";
        case KEY_NUMPAD3:
            return "numpad_3";
        case KEY_NUMPAD4:
            return "numpad_4";
        case KEY_NUMPAD5:
            return "numpad_5";
        case KEY_NUMPAD6:
            return "numpad_6";
        case KEY_NUMPAD7:
            return "numpad_7";
        case KEY_NUMPAD8:
            return "numpad_8";
        case KEY_NUMPAD9:
            return "numpad_9";
        case KEY_MULTIPLY:
            return "numpad_mult";
        case KEY_ADD:
            return "numpad_add";
        case KEY_SEPARATOR:
            return "numpad_sep";
        case KEY_SUBTRACT:
            return "numpad_sub";
        case KEY_DECIMAL:
            return "numpad_decimal";
        case KEY_DIVIDE:
            return "numpad_div";

        case KEY_F1:
            return "f1";
        case KEY_F2:
            return "f2";
        case KEY_F3:
            return "f3";
        case KEY_F4:
            return "f4";
        case KEY_F5:
            return "f5";
        case KEY_F6:
            return "f6";
        case KEY_F7:
            return "f7";
        case KEY_F8:
            return "f8";
        case KEY_F9:
            return "f9";
        case KEY_F10:
            return "f10";
        case KEY_F11:
            return "f11";
        case KEY_F12:
            return "f12";

        case KEY_NUMLOCK:
            return "num_lock";
        case KEY_SCRLOCK:
            return "scroll_lock";
        //case KEY_NUMPAD_EQUAL:
        //    return "numpad_equal";

        case KEY_LSHIFT:
            return "l_shift";
        case KEY_RSHIFT:
            return "r_shift";
        case KEY_LCONTROL:
            return "l_ctrl";
        case KEY_RCONTROL:
            return "r_ctrl";
        case KEY_LALT:
            return "l_alt";
        case KEY_RALT:
            return "r_alt";

#if 0
        case KEY_SEMICOLON:
            return ";";

        case KEY_APOSTROPHE:
            return "'";
        case KEY_EQUAL:
            return "=";
        case KEY_COMMA:
            return ",";
        case KEY_MINUS:
            return "-";
        case KEY_PERIOD:
            return ".";
        case KEY_SLASH:
            return "/";

        case KEY_GRAVE:
            return "`";

        case KEY_LBRACKET:
            return "[";
        case KEY_PIPE:
            return "\\";
        case KEY_RBRACKET:
            return "]";
#endif

        default:
            return "undefined";
    }
}