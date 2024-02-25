#include "Engine.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Core/Event.h"
#include "Engine/Core/Input.h"

#include "Engine/Renderer/Renderer.h"
#include "Engine/Resources/Resource_Manager.h"

struct RohinEngine {
    RohinApp* app;

    real32 target_frame_time;
    real32 last_frame_time;
    bool32 is_running;
    bool32 is_paused;
    bool32 lock_framerate;

    uint8* engine_memory;
    uint64 engine_memory_size;
    memory_arena engine_arena;
    memory_arena resource_arena;
    memory_arena frame_render_arena;

    // tmp, should probably be pulled out into a 'scene' representation
    real32 view_vert_fov;
    laml::Mat4 projection_matrix;
};
global_variable RohinEngine engine;

bool32 engine_on_resize(uint16 code, void* sender, void* listener, event_context context) {
    real32 width  = (real32)context.u32[0];
    real32 height = (real32)context.u32[1];
    laml::transform::create_projection_perspective(engine.projection_matrix, engine.view_vert_fov, width/height, 0.1f, 100.0f);

    renderer_resized(context.u32[0], context.u32[1]);
    engine.app->on_resize(engine.app, context.u32[0], context.u32[1]);

    return false;
}

bool32 engine_on_event(uint16 code, void* sender, void* listener, event_context context) {
    switch (code) {
        case EVENT_CODE_APPLICATION_QUIT:
            engine.is_running = false;
            return true;
        case EVENT_CODE_KEY_PRESSED:
            if (context.u16[0] == KEY_ESCAPE) {
                event_context no_data = {};
                event_fire(EVENT_CODE_APPLICATION_QUIT, 0, no_data);
            }
    }

    if ((code != EVENT_CODE_KEY_PRESSED)) {
        RH_TRACE("Engine[0x%016llX] recieved event code %d \n         "
                 "Sender=[0x%016llX] \n         "
                 "Listener=[0x%016llX] \n         "
                 "Data=[%llu], [%u,%u], [%hu,%hu,%hu,%hu]",
                 (uintptr_t)(&engine), code, (uintptr_t)sender, (uintptr_t)listener,
                 context.u64,
                 context.u32[0], context.u32[1],
                 context.u16[0], context.u16[1], context.u16[2], context.u16[3]);
    }

    return false;
}

bool32 start_rohin_engine(RohinApp* app) {
    engine.app = app;
    
    // get memory for the application
    // initialize all systems we need to
    InitLogging();

    RH_INFO("Rohin Engine v0.0.1");

    AppConfig config;
    config.application_name = app->app_config.application_name;
    config.start_x = app->app_config.start_x;
    config.start_y = app->app_config.start_y;
    config.start_width = app->app_config.start_width;
    config.start_height = app->app_config.start_height;
    config.requested_memory = app->app_config.requested_permanant_memory + 
                              app->app_config.requested_transient_memory;
    if (!platform_startup(&config)) {
        RH_FATAL("Failed on platform startup!");
        return false;
    }

#if ROHIN_INTERNAL
    uint64 base_address = Terabytes(2);
#else
    uint64 base_address = 0;
#endif

    engine.engine_memory_size = Megabytes(64);
    engine.engine_memory = (uint8*)platform_alloc(engine.engine_memory_size, 0);
    CreateArena(&engine.frame_render_arena, Megabytes(1),  engine.engine_memory);
    CreateArena(&engine.engine_arena,       Megabytes(15),  engine.engine_memory + Megabytes(1));
    CreateArena(&engine.resource_arena,     Megabytes(48), engine.engine_memory + Megabytes(16));

    uint32 monitor_refresh_hz = 60;
    uint32 target_framerate = 60;

    // renderer startup
    if (!renderer_initialize(&engine.engine_arena, config.application_name, nullptr)) {
        RH_FATAL("Failed to initialize renderer!");
        return false;
    }
    // tmp: remove this!
    renderer_resized(config.start_width, config.start_height);

    // resource system startup
    if (!resource_init(&engine.resource_arena)) {
        RH_FATAL("Failed to initialize resource manager!");
        return false;
    }

    // after resource system is setup
    if (!renderer_create_pipeline()) {
        RH_FATAL("Failed to create render pipeline!");
        return false;
    }

    render_texture_2D texture;
    //resource_load_texture_file("Data/Images/Stormtrooper_D.png", &texture);
    //resource_load_texture_file("Data/Images/grid/PNG/Orange/texture_08.png", &texture);
    //resource_load_texture_file("Data/Images/copper/albedo.png", &texture);
    //resource_load_texture_file("Data/Images/frog.png", &texture);
    resource_load_texture_file("Data/Images/waffle/WaffleSlab2_albedo.png", &texture);

    engine.target_frame_time = 1.0f / ((real32)target_framerate);
    engine.last_frame_time = engine.target_frame_time;
    engine.lock_framerate = true;

    // Initialize some systems
    event_init(&engine.engine_arena);
    event_register(EVENT_CODE_APPLICATION_QUIT, 0, engine_on_event);
    event_register(EVENT_CODE_KEY_PRESSED, 0, engine_on_event);
    //event_register(EVENT_CODE_MOUSE_MOVED, 0, engine_on_event);
    event_register(EVENT_CODE_RESIZED, 0, engine_on_resize);

    input_init(&engine.engine_arena);

    void* memory = platform_alloc(config.requested_memory, base_address);
    if (memory) {
        engine.app->memory.PermanentStorage = memory;
        engine.app->memory.PermanentStorageSize = engine.app->app_config.requested_permanant_memory;
        engine.app->memory.TransientStorage = ((uint8*)memory + engine.app->memory.PermanentStorageSize);
        engine.app->memory.TransientStorageSize = engine.app->app_config.requested_transient_memory;

        engine.is_running = true;

        engine.app->startup(engine.app);

        uint64 LastCounter = platform_get_wall_clock();
        uint64 FlipWallClock = platform_get_wall_clock();
        //uint64 LastCycleCount = __rdtsc();

        real32 AR = (real32)config.start_width / (real32)config.start_height;
        engine.view_vert_fov = 75.0f;
        laml::transform::create_projection_perspective(engine.projection_matrix, engine.view_vert_fov, AR, 0.1f, 100.0f);

        engine.app->initialize(engine.app);

        // Game Loop!
        while(engine.is_running) {
            if (!platform_process_messages()) {
                engine.is_running = false;
            }

            if (!engine.is_paused) {
                // construct render packet with all the things that are visible this frame
                ResetArena(&engine.frame_render_arena);
                render_packet* packet = PushStruct(&engine.frame_render_arena, render_packet);
                packet->arena = &engine.frame_render_arena;

                packet->delta_time = engine.last_frame_time;
                packet->projection_matrix = engine.projection_matrix;
                packet->col_grid = nullptr;

                engine.app->update_and_render(engine.app, packet, engine.last_frame_time);

                renderer_draw_frame(packet);

                uint64 WorkCounter = platform_get_wall_clock();
                real32 WorkSecondsElapsed = (real32)platform_get_seconds_elapsed(LastCounter, WorkCounter);

                // TODO: Untested! buggy
                real32 SecondsElapsedForFrame = WorkSecondsElapsed;
                bool32 LockFramerate = true;
                if (LockFramerate) {
                    if (SecondsElapsedForFrame < engine.target_frame_time) {
                        uint64 SleepMS = (uint64)(1000.0f*(engine.target_frame_time - SecondsElapsedForFrame));
                        if (SleepMS > 0) {
                            platform_sleep(SleepMS);
                        }
    
                        while (SecondsElapsedForFrame < engine.target_frame_time) {
                            SecondsElapsedForFrame = (real32)platform_get_seconds_elapsed(LastCounter, platform_get_wall_clock());
                        }
                    } else {
                        // TODO: Missed Frame Rate!
                        RH_ERROR("Frame rate missed!");
                    }
                }

                uint64 EndCounter = platform_get_wall_clock();
                engine.last_frame_time = (real32)platform_get_seconds_elapsed(LastCounter, EndCounter);
                real32 MSPerFrame = (real32)(1000.0f * engine.last_frame_time);
                //MSLastFrame.addSample(MSPerFrame);
                LastCounter = EndCounter;
    
                //Win32DisplayBufferToWindow(DeviceContext, Dimension.Width, Dimension.Height);
                platform_swap_buffers();
    
                FlipWallClock = platform_get_wall_clock();

                //uint64 EndCycleCount = __rdtsc();
                //uint64 CyclesElapsed = EndCycleCount - LastCycleCount;
                //LastCycleCount = EndCycleCount;
    
                real32 FPS = 1000.0f / MSPerFrame;
                //real32 MCPF = ((real32)CyclesElapsed / (1000.0f*1000.0f));
    
#if 0
                RH_TRACE("Frame: %.02f ms  %.02ffps\n", MSPerFrame, FPS);
#else
                platform_console_set_title("%s: %.02f ms, FPS: %.02ffps", config.application_name, MSPerFrame, FPS);
#endif

                input_update(engine.last_frame_time);
            }
        }

        engine.app->shutdown(engine.app);

        platform_free(memory);
    } else {
        RH_FATAL("Requested %u bytes of memory, but could not get it from the os!", config.requested_memory);
    }

    renderer_shutdown();
    platform_shutdown();

    // shutdown all systems
    input_shutdown();
    event_shutdown();
    ShutdownLogging();

    return true;
}