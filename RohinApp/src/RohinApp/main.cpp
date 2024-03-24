#include "rohin_game.h"

#define TEST 0
#if !defined(TEST) || TEST == 0

#include <Engine/Core/Config.h>
#include <Engine/Memory/Memory_Arena.h>
#include <Engine/Memory/Memory.h>
#include <Engine/Core/String.h>

#include <Engine/Application.h>
bool32 create_application(RohinApp* app);
#include <Engine/Entry_Point.h>

bool32 create_application(RohinApp* app) {
    uint8 arena_buffer[2048];
    memory_zero(arena_buffer, sizeof(arena_buffer));
    memory_arena arena;
    CreateArena(&arena, sizeof(arena_buffer), arena_buffer);
    const char* config_filename = "Game/run_tree/startup.conf";
    config_file config = parse_config_from_file(&arena, config_filename, "startup.conf");

    if (config.load_successful) {
        char* app_name = get_config_string(&config, "app_config.app_name", "default_name");
        uint64 name_len = string_length(app_name);
        memory_copy(app->app_config.application_name, app_name, name_len);

        app->app_config.start_x      = get_config_int(&config, "app_config.start_x", 10);
        app->app_config.start_y      = get_config_int(&config, "app_config.start_y", 10);
        app->app_config.start_width  = get_config_int(&config, "app_config.start_width", 800);
        app->app_config.start_height = get_config_int(&config, "app_config.start_height", 600);

        app->app_config.requested_permanant_memory = Megabytes(get_config_int(&config, "app_config.memory.requested_permanant_memory", 32));
        app->app_config.requested_transient_memory = Megabytes(get_config_int(&config, "app_config.memory.requested_transient_memory", 256));
    } else {
        memory_copy(app->app_config.application_name, "Rohin App", 10);

        app->app_config.start_x = 10;
        app->app_config.start_y = 10;
        app->app_config.start_width = 800;
        app->app_config.start_height = 600;

        app->app_config.requested_permanant_memory = Megabytes(32);
        app->app_config.requested_transient_memory = Megabytes(256);
    }

    app->startup = game_startup;
    app->initialize = game_initialize;
    app->update_and_render = game_update_and_render;
    app->on_resize = game_on_resize;
    app->shutdown = game_shutdown;

    return true;
}

#elif TEST == 1
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <Engine/Core/Config.h>

#include <Engine/Memory/Memory.h>
#include <Engine/Memory/Memory_Arena.h>
#include <Engine/Core/Logger.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Core/String.h>
#include <Engine/Core/Timing.h>


//int WinMain(void* instance, void* prev_instance, char* cmd_line, int show_cmd) {
int WinMain(HINSTANCE instance, HINSTANCE prev_instance, char* cmd_line, int show_cmd) {
    InitLogging(false, LOG_LEVEL_DEBUG);
    RH_TRACE("starting...");

    uint8 arena_buffer[32*1024];
    memory_zero(arena_buffer, sizeof(arena_buffer));
    memory_arena arena;
    CreateArena(&arena, sizeof(arena_buffer), arena_buffer);

    const char* filename = "../Game/run_tree/startup.conf";
    file_handle file = platform_read_entire_file(filename);
    if (!file.num_bytes) {
        RH_ERROR("Failed to read resource file");
        platform_sleep(500);
        return -1;
    }
    RH_TRACE("File '%s' read: %llu bytes", filename, file.num_bytes);

    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);

    QueryPerformanceCounter(&start);
    config_file config = parse_config_from_file(&arena, filename, "startup.conf");
    QueryPerformanceCounter(&end);

    double elapsed_time = (double)(end.QuadPart - start.QuadPart) / ((double)freq.QuadPart);
    RH_INFO("%f ms to parse.", elapsed_time*1000.0f);

    platform_free_file_data(&file);
    memory_zero(file.data, file.num_bytes); // just so we know for sure the config_file owns its own data

    // now extract values from it
    RH_INFO("app_name = '%s'",   get_config_string(&config, "app_config.app_name",  "default app name"));
    RH_INFO("start_x = %d",      get_config_int(&config, "app_config.start_x",      0));
    RH_INFO("start_y = %d",      get_config_int(&config, "app_config.start_y",      0));
    RH_INFO("start_width = %d",  get_config_int(&config, "app_config.start_width",  100));
    RH_INFO("start_height = %d", get_config_int(&config, "app_config.start_height", 100));

    RH_INFO("requested_permanant_memory = %d",   get_config_int(&config, "app_config.memory.requested_permanant_memory", 1));
    RH_INFO("requested_transient_memory = %d",   get_config_int(&config, "app_config.memory.requested_transient_memory", 1));

    platform_sleep(2500);
    return 0;
}
#elif TEST==2

#include <stdlib.h>
#include <stdio.h>

#include <Engine/Memory/Memory.h>
#include <Engine/Memory/Memory_Arena.h>

const int64 DYNARRAY_ARENA    = -4;
const int64 DYNARRAY_CAPACITY = -3;
const int64 DYNARRAY_STRIDE   = -2;
const int64 DYNARRAY_COUNT    = -1;
const int64 DYNARRAY_DATA     =  0;
constexpr uint64 HEADER_SIZE = 4*sizeof(int64);

void print_arr(uint32* uint32_arr) {
    uint64 arr_count = GetArrayCount(uint32_arr);
    uint64 arr_capacity = GetArrayCapacity(uint32_arr);
    printf("array: [%llu/%llu]\n", arr_count, arr_capacity);

    printf("data = {");
    uint64 n;
    for (n = 0; n < arr_count; n++) {
        printf("%u, ", uint32_arr[n]);
    }
    for (n = arr_count; n < arr_capacity; n++) {
        printf("-, ");
    }
    printf("}\n");
}

struct data_t {
    real32 v1;
    uint64 v2;
    uint8 v3;
    uint64 v4;
};
void print_arr(data_t* arr) {
    uint64 arr_count = GetArrayCount(arr);
    uint64 arr_capacity = GetArrayCapacity(arr);
    printf("array: [%llu/%llu]\n", arr_count, arr_capacity);

    printf("data = {\n");
    uint64 n;
    for (n = 0; n < arr_count; n++) {
        printf(" { %.1f, %llu, %c, %llu }, \n", arr[n].v1, arr[n].v2, arr[n].v3, arr[n].v4);
    }
    for (n = arr_count; n < arr_capacity; n++) {
        printf(" {-}, \n");
    }
    printf("}\n");
}

//int WinMain(void* instance, void* prev_instance, char* cmd_line, int show_cmd) {
int main(int argc, char** argv) {
    printf("running...\n");

    void* data = malloc(Megabytes(1));

    memory_arena arena;
    CreateArena(&arena, Megabytes(1), (uint8*)data);

    // create a darray of uint32
    uint32* uint32_arr = CreateArray(&arena, uint32, 5);
    ArrayReserve(uint32_arr, 20);

    // try accessing now!
    print_arr(uint32_arr);

    // add a new element
    ArrayPush(uint32_arr, 5);
    ArrayPush(uint32_arr, 1);
    ArrayPush(uint32_arr, 3);

    print_arr(uint32_arr);

    ArrayPush(uint32_arr, 2);
    ArrayPush(uint32_arr, 7);

    print_arr(uint32_arr);

    ArrayResize(uint32_arr, 2);

    print_arr(uint32_arr);

    ArrayPush(uint32_arr, 10);
    ArrayPush(uint32_arr, 1);

    print_arr(uint32_arr);

    ArrayResize(uint32_arr, 5);

    print_arr(uint32_arr);

    ArrayPush(uint32_arr, 6);
    ArrayPush(uint32_arr, 6);
    ArrayPush(uint32_arr, 6);

    print_arr(uint32_arr);

    for (uint32 n = 0; n < 20; n++) {
        ArrayPush(uint32_arr, 100);
        print_arr(uint32_arr);
    }

    ArrayClear(uint32_arr);
    print_arr(uint32_arr);

    // create a darray of a complex struct
    printf("sizeof(data_t) = %llu\n", sizeof(data_t));

    data_t* data_arr = CreateArray(&arena, data_t, 5);
    ArrayReserve(uint32_arr, 10);

    print_arr(data_arr);

    data_t d { 1, 2, 3, 4 };
    ArrayPush(data_arr, d);
    data_t d2 { 4, 3, 2, 1 };
    ArrayPush(data_arr, d2);

    print_arr(data_arr);

    data_t* end = ArrayPeek(data_arr);
    print_arr(data_arr);
    end->v1 = 10.0f;
    print_arr(data_arr);

    free(data);
    system("pause");
    return 0;
}

#endif