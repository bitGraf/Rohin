#include "rohin_game.h"

#include <Engine/Core/Logger.h>
#include <Engine/Memory/Memory_Arena.h>
#include <Engine/Renderer/Render_Types.h>
#include <Engine/Resources/Resource_Manager.h>

struct game_state {
    memory_arena perm_arena;
    memory_arena mesh_arena;

    uint32 num_geometry;
    triangle_geometry* geometry;
};

bool32 game_startup(RohinApp* app) {
    RH_INFO("Game startup.");

    game_state* state = (game_state*)(app->memory.PermanentStorage);
    CreateArena(&state->perm_arena, app->memory.PermanentStorageSize, (uint8*)app->memory.PermanentStorage + sizeof(game_state));
    state->mesh_arena = CreateSubArena(&state->perm_arena, Megabytes(1));

    state->num_geometry = 1;
    state->geometry = PushArray(&state->mesh_arena, triangle_geometry, state->num_geometry);

    app->memory.IsInitialized = true;

    return true;
}

bool32 game_initialize(RohinApp* app) {
    RH_INFO("Game initialize.");

    game_state* state = (game_state*)(app->memory.PermanentStorage);

    resource_load_mesh_file("Data/Models/dance.mesh", &state->geometry[0], 0, 0, 0);

    return true;
}

bool32 game_update_and_render(RohinApp* app, render_packet* packet, real32 delta_time) {
    game_state* state = (game_state*)(app->memory.PermanentStorage);

    // simulate game state

    // ...

    // push all the render commands to the render_packet
    packet->num_commands = state->num_geometry;
    packet->commands = PushArray(packet->arena, render_command, packet->num_commands);
    for (uint32 n = 0; n < packet->num_commands; n++) {
        packet->commands[n].model_matrix = laml::Mat4(1.0f);
        packet->commands[n].geom = state->geometry[n];
        packet->commands[n].material_handle = 0;
    }

    return true;
}

void game_on_resize(RohinApp* app, uint32 new_width, uint32 new_height) {
    RH_INFO("Game on resize [%dx%d].", new_width, new_height);
}

void game_shutdown(RohinApp* app) {
    RH_INFO("Game shutdown.");
}
