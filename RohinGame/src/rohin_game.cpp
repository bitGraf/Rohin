#include "rohin_game.h"

#include <Engine/Core/Logger.h>
#include <Engine/Memory/Memory_Arena.h>
#include <Engine/Renderer/Render_Types.h>
#include <Engine/Resources/Resource_Manager.h>
#include <Engine/Core/Input.h>
#include <Engine/Core/Event.h>

#include <Engine/Collision/Collision.h>

#include <Engine/Renderer/Renderer.h>

struct player_state {
    laml::Vec3 position;
    laml::Quat orientation;
};

struct game_state {
    memory_arena perm_arena;
    memory_arena trans_arena;
    memory_arena mesh_arena;

    triangle_geometry* level_geom;
    triangle_geometry* player_geom;

    uint32 num_geometry;
    triangle_geometry* geometry;

    player_state player;

    collision_grid grid;
};

bool32 on_key_event(uint16 code, void* sender, void* listener, event_context context) {
    game_state* state = (game_state*)listener;

    uint16 key_code = context.u16[0];
    if (key_code == KEY_P) {
        laml::Vec3 pos = state->player.position;
        RH_INFO("Player position: [%4.1f,%4.1f,%4.1f]", pos.x, pos.y, pos.z);
    }
    //RH_TRACE("Game[0x%016llX] recieved event code %d \n         "
    //         "Sender=[0x%016llX] \n         "
    //         "Listener=[0x%016llX] \n         "
    //         "Data=[%llu], [%u,%u], [%hu,%hu,%hu,%hu]",
    //         state, code, (uintptr_t)sender, (uintptr_t)listener,
    //         context.u64,
    //         context.u32[0], context.u32[1],
    //         context.u16[0], context.u16[1], context.u16[2], context.u16[3]);

    return false;
}

bool32 game_startup(RohinApp* app) {
    RH_INFO("Game startup.");

    game_state* state = (game_state*)(app->memory.PermanentStorage);
    CreateArena(&state->perm_arena, app->memory.PermanentStorageSize, (uint8*)app->memory.PermanentStorage + sizeof(game_state));
    state->mesh_arena = CreateSubArena(&state->perm_arena, Megabytes(1));

    CreateArena(&state->trans_arena, app->memory.TransientStorageSize, (uint8*)app->memory.TransientStorage);

    state->level_geom = PushStruct(&state->mesh_arena, triangle_geometry);
    state->player_geom = nullptr;
    
    state->num_geometry = 0;
    state->geometry = nullptr;

    app->memory.IsInitialized = true;

    event_register(EVENT_CODE_KEY_PRESSED, state, on_key_event);

    return true;
}

bool32 game_initialize(RohinApp* app) {
    RH_INFO("Game initialize.");

    game_state* state = (game_state*)(app->memory.PermanentStorage);
    resource_load_mesh_file("Data/Models/dance.mesh", state->player_geom, 0, 0, 0);

    // load the level geometry into the collision grid
    // 32x256x256, centered on (0,0,0)
    // grid_size of 1
    collision_create_grid(&state->trans_arena, &state->grid, { 20.0f, -0.5f, -5.0f }, 1.0f, 64, 32, 64);
    //collision_create_grid(&state->trans_arena, &state->grid, { 0.0f, 0.0f, 0.0f }, 1.0f, 64, 32, 64);
    resource_load_mesh_file_for_level("Data/Models/level1.mesh", state->level_geom, &state->grid);
    collision_grid_finalize(&state->trans_arena, &state->grid);

    state->player.position = {0.0f, 1.0f, 0.0f};
    state->player.orientation = {0.0f, 0.0f, 0.0f, 1.0f};

    return true;
}

bool32 game_update_and_render(RohinApp* app, render_packet* packet, real32 delta_time) {
    game_state* state = (game_state*)(app->memory.PermanentStorage);

    // simulate game state
    laml::Mat4 eye(1.0f);
    int32 mouse_x, mouse_y;
    input_get_mouse_pos(&mouse_x, &mouse_y);
    real32 yaw = -0.75f * ((real32)mouse_x);
    real32 pitch = -0.35f * (real32)mouse_y;

    // TODO: this is a silly way to go, why not just ypr->quat
    laml::Mat3 player_rot;
    laml::transform::create_transform_rotation(player_rot, yaw, pitch, 0.0f);
    state->player.orientation = laml::transform::quat_from_mat(player_rot);

    laml::Vec3 right = player_rot._cols[0];
    laml::Vec3 up = player_rot._cols[1];
    laml::Vec3 forward = -player_rot._cols[2];
    laml::Vec3 vel(0.0f);
    real32 speed = input_is_key_down(KEY_LSHIFT) ? 10.0f : 2.5f;
    if (input_is_key_down(KEY_W)) {
        vel = forward;
    } else if (input_is_key_down(KEY_S)) {
        vel = -forward;
    }
    if (input_is_key_down(KEY_D)) {
        vel = vel + right;
    } else if (input_is_key_down(KEY_A)) {
        vel = vel - right;
    }
    if (input_is_key_down(KEY_SPACE)) {
        vel = vel + up;
    } else if (input_is_key_down(KEY_LCONTROL)) {
        vel = vel - up;
    }
    state->player.position = state->player.position + (speed * vel * delta_time);

    laml::Mat4 player_transform;
    laml::transform::create_transform(player_transform, state->player.orientation, state->player.position);

    // ...

    // push all the render commands to the render_packet
    packet->num_commands = 1;
    packet->commands = PushArray(packet->arena, render_command, packet->num_commands);
    
    packet->commands[0].model_matrix = eye;
    packet->commands[0].geom = *state->level_geom;
    packet->commands[0].material_handle = 0;
#if 0
    packet->num_commands = state->num_geometry;
    packet->commands = PushArray(packet->arena, render_command, packet->num_commands);
    for (uint32 n = 0; n < packet->num_commands; n++) {
        packet->commands[n].model_matrix = transform;
        packet->commands[n].geom = state->geometry[n];
        packet->commands[n].material_handle = 0;
    }
#endif

#if 1
    packet->col_grid = &state->grid;
#endif

    // calculate view-point
    packet->camera_pos = state->player.position;
    packet->camera_orientation = state->player.orientation;

    return true;
}

void game_on_resize(RohinApp* app, uint32 new_width, uint32 new_height) {
    RH_INFO("Game on resize [%dx%d].", new_width, new_height);
}

void game_shutdown(RohinApp* app) {
    RH_INFO("Game shutdown.");
}
