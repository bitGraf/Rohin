#pragma once

#include "Engine/Defines.h"

struct AppConfig {
    const char* application_name;
    int32 start_x;
    int32 start_y;
    int32 start_width;
    int32 start_height;

    uint64 requested_memory;
};

/*
 * Platform Layer:
 *   provides services to higher layers
 * 
 * exported functions:
 *   startup()
 *   shutdown()
 *   process_messages()
 *   platform_alloc() <= returns zeroed memory :)
 *   platform_free()
 *   write to console
 *   get_time()
 *   sleep()
 * */

bool32 platform_startup(AppConfig* config);
void platform_shutdown();
bool32 platform_process_messages();
void* platform_alloc(uint64 size, uint64 base_address);
void platform_free(void* memory);
void platform_console_write_error(const char* Message, uint8 Color);
void platform_console_write(const char* Message, uint8 Color);
void platform_console_set_title(const char* Message, ...);
int64 platform_get_wall_clock();
real64 platform_get_seconds_elapsed(int64 start, int64 end);
void platform_sleep(uint64 ms);

// rendering stuff
void platform_swap_buffers();