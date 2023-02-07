#pragma once

#include "Engine/Defines.h"

enum mouse_button_codes {
    BUTTON_LEFT = 0,
    BUTTON_RIGHT,
    BUTTON_MIDDLE,

    BUTTON_MOUSE4,
    BUTTON_MOUSE5,

    BUTTON_MAX_BUTTONS
};

/* 
 * Codes based off win32 VK Codes:
 * https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
 * */
enum keyboard_keys {
    KEY_BACKSPACE = 0x08,
    KEY_TAB = 0x09,
    KEY_RETURN = 0x0D,
    KEY_SHIFT = 0x10,
    KEY_CONTROL = 0x11,
    KEY_ALT = 0x12,
    KEY_PAUSE = 0x13,
    KEY_CAPSLOCK = 0x14,
    KEY_ESCAPE = 0x1B,
    KEY_SPACE = 0x20,
    KEY_PAGEUP = 0x21,
    KEY_PAGEDN = 0x22,
    KEY_END = 0x23,
    KEY_HOME = 0x24,

    KEY_LEFT = 0x25,
    KEY_UP = 0x26,
    KEY_RIGHT = 0x27,
    KEY_DOWN = 0x28,

    KEY_PRINTSCR = 0x2C,
    KEY_INSERT = 0x2D,
    KEY_DELETE = 0x2E,
    
    KEY_0 = 0x30,
    KEY_1 = 0x31,
    KEY_2 = 0x32,
    KEY_3 = 0x33,
    KEY_4 = 0x34,
    KEY_5 = 0x35,
    KEY_6 = 0x36,
    KEY_7 = 0x37,
    KEY_8 = 0x38,
    KEY_9 = 0x39,

    KEY_A = 0x41,
    KEY_B = 0x42,
    KEY_C = 0x43,
    KEY_D = 0x44,
    KEY_E = 0x45,
    KEY_F = 0x46,
    KEY_G = 0x47,
    KEY_H = 0x48,
    KEY_I = 0x49,
    KEY_J = 0x4A,
    KEY_K = 0x4B,
    KEY_L = 0x4C,
    KEY_M = 0x4D,
    KEY_N = 0x4E,
    KEY_O = 0x4F,
    KEY_P = 0x50,
    KEY_Q = 0x51,
    KEY_R = 0x52,
    KEY_S = 0x53,
    KEY_T = 0x54,
    KEY_U = 0x55,
    KEY_V = 0x56,
    KEY_W = 0x57,
    KEY_X = 0x58,
    KEY_Y = 0x59,
    KEY_Z = 0x5A,

    KEY_NUMPAD0 = 0x60,
    KEY_NUMPAD1 = 0x61,
    KEY_NUMPAD2 = 0x62,
    KEY_NUMPAD3 = 0x63,
    KEY_NUMPAD4 = 0x64,
    KEY_NUMPAD5 = 0x65,
    KEY_NUMPAD6 = 0x66,
    KEY_NUMPAD7 = 0x67,
    KEY_NUMPAD8 = 0x68,
    KEY_NUMPAD9 = 0x69,

    KEY_MULTIPLY = 0x6A,
    KEY_ADD = 0x6B,
    KEY_SEPARATOR = 0x6C,
    KEY_SUBTRACT = 0x6D,
    KEY_DECIMAL = 0x6E,
    KEY_DIVIDE = 0x6F,

    KEY_F1 = 0x70,
    KEY_F2 = 0x71,
    KEY_F3 = 0x72,
    KEY_F4 = 0x73,
    KEY_F5 = 0x74,
    KEY_F6 = 0x75,
    KEY_F7 = 0x76,
    KEY_F8 = 0x77,
    KEY_F9 = 0x78,
    KEY_F10 = 0x79,
    KEY_F11 = 0x7A,
    KEY_F12 = 0x7B,

    KEY_NUMLOCK = 0x90,
    KEY_SCRLOCK = 0x91,

    KEY_LSHIFT = 0xA0,
    KEY_RSHIFT = 0xA1,
    KEY_LCONTROL = 0xA2,
    KEY_RCONTROL = 0xA3,
    KEY_LALT = 0xA4,
    KEY_RALT = 0xA5,

    // more to add

    KEY_MAX_KEYS = 0xFF
};

bool32 input_init(struct memory_arena* arena);
void input_shutdown();

void input_update(real32 delta_time);

// ask for the state of keys
RHAPI bool32 input_is_key_down(keyboard_keys key);
RHAPI bool32 input_is_key_up(keyboard_keys key);
RHAPI bool32 input_was_key_down(keyboard_keys key);
RHAPI bool32 input_was_key_up(keyboard_keys key);

void input_process_key(keyboard_keys key, uint8 pressed);

// ask for state of mouse
RHAPI bool32 input_is_button_down(mouse_button_codes button);
RHAPI bool32 input_is_button_up(mouse_button_codes button);
RHAPI bool32 input_was_button_down(mouse_button_codes button);
RHAPI bool32 input_was_button_up(mouse_button_codes button);

RHAPI void input_get_mouse_pos(int32* x, int32* y);
RHAPI void input_get_prev_mouse_pos(int32* x, int32* y);
RHAPI void input_get_raw_mouse_offset(int32* dx, int32* dy);

void input_process_mouse_button(mouse_button_codes button, uint8 pressed);
void input_process_mouse_move(int32 mouse_x, int32 mouse_y);
void input_process_mouse_wheel(int32 mouse_z);

void input_process_raw_mouse_move(int32 mouse_dx, int32 mouse_dy);

RHAPI const char* input_get_key_string(keyboard_keys key);