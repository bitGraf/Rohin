#include "Platform.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"
#include "Engine/Core/Event.h"
#include "Engine/Memory/MemoryUtils.h"
#include "Engine/Core/Input.h"


#ifdef RH_PLATFORM_LINUX

bool32 platform_startup(AppConfig* config) { 
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
    return false; 
    }
void platform_shutdown() {
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
}
bool32 platform_process_messages() { 
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
    return false; 
}
void* platform_alloc(uint64 size, uint64 base_address) {
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
    return nullptr; 
}
void platform_free(void* memory) {
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
}
void platform_console_write_error(const char* Message, uint8 Color) {
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
}
void platform_console_write(const char* Message, uint8 Color) {
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
}
void platform_console_set_title(const char* Message, ...) {
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
}
int64 platform_get_wall_clock() { 
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
    return 0; 
}
real64 platform_get_seconds_elapsed(int64 start, int64 end) { 
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
    return 0.0;
}
void platform_sleep(uint64 ms) {
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
}
void platform_update_mouse() {
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
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
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
    return nullptr;
}
void* memory_copy(void* dest, const void* src, uint64 size) {
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
    return nullptr;
}
void* memory_set(void* memory, uint8 value, uint64 size) { 
    #pragma message ( "NOT YET IMPLEMENTED FOR LINUX" )
    return nullptr;
}

#endif