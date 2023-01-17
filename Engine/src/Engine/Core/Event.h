#pragma once

#include "Engine/Defines.h"

struct event_context {
#if 0
    // 128 bytes
    union {
        int64  i64[2];
        uint64 u64[2];
        real64 f64[2];

        int32  i32[4];
        uint32 u32[4];
        real32 f32[4];

        int16  i16[8];
        uint16 u16[8];

        int8   i8[16];
        uint8  u8[16];

        char   c[16];
    };
#else
    // 64 bytes
    union {
        int64  i64;
        uint64 u64;
        real64 f64;

        int32  i32[2];
        uint32 u32[2];
        real32 f32[2];

        int16  i16[4];
        uint16 u16[4];

        int8   i8[8];
        uint8  u8[8];

        char   c[8];
    };
#endif
};

// callback should return true if handled. (i.e. don't propoagte the message anymore)
typedef bool32 (*on_event_func)(uint16 code, void* sender, void* listener, event_context data);

bool32 event_init(struct memory_arena* arena);
void event_shutdown();

RHAPI bool32 event_register(uint16 code, void* listener, on_event_func on_event);
RHAPI bool32 event_unregister(uint16 code, void* listener, on_event_func on_event);

RHAPI bool32 event_fire(uint16 code, void* sender, event_context context);

enum system_event_code {
    // shut down app on the next frame
    EVENT_CODE_APPLICATION_QUIT = 0x01,

    /* Context usage: 
     * key_code in u16[0];
     * */
    EVENT_CODE_KEY_PRESSED = 0x02,
    EVENT_CODE_KEY_RELEASED = 0x03,
    EVENT_CODE_BUTTON_PRESSED = 0x04,
    EVENT_CODE_BUTTON_RELEASED = 0x05,

    /* Context usage: 
     * mouse_x in u16[0];
     * mouse_y in u16[1];
     * */
    EVENT_CODE_MOUSE_MOVED = 0x06,
    /* Context usage: 
     * z_delta in u8[0];
     * */
    EVENT_CODE_MOUSE_WHEEL = 0x07,

    /* Context usage: 
     * width in  u16[0];
     * height in u16[1];
     * */
    EVENT_CODE_RESIZED = 0x08,

    MAX_EVENT_CODE = 0xFF
};