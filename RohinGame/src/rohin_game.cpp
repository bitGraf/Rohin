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

    real32 scale;

    real32 pitch;
    real32 yaw;

    real32 height;
    real32 radius;
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
    player_state debug_camera;

    collision_grid grid;
    uint32 gx, gy, gz;

    collision_triangle triangle;
    triangle_geometry tri_geom;

    collision_capsule capsule;
    triangle_geometry capsule_geom;
    collision_sector sector;

    real32 theta;
    bool32 paused;
};

bool32 on_key_event(uint16 code, void* sender, void* listener, event_context context) {
    game_state* state = (game_state*)listener;

    uint16 key_code = context.u16[0];
    if (key_code == KEY_RIGHT) {
        if (input_is_key_down(KEY_LSHIFT)) {
            collision_grid_cell* cell = &state->grid.cells[state->gx][state->gy][state->gz];
            do {
                state->gz++;
                if (state->gz == state->grid.num_z) {
                    state->gz = 0;
                    state->gy++;
                    if (state->gy == state->grid.num_y) {
                        state->gy = 0;
                        state->gx++;
                        if (state->gx == state->grid.num_x) {
                            state->gx = 0;
                        }
                    }
                }
                cell = &state->grid.cells[state->gx][state->gy][state->gz];
            } while (cell->num_surfaces == 0);
        } else {
            state->gz++;
            if (state->gz == state->grid.num_z) {
                state->gz = 0;
                state->gy++;
                if (state->gy == state->grid.num_y) {
                    state->gy = 0;
                    state->gx++;
                    if (state->gx == state->grid.num_x) {
                        state->gx = 0;
                    }
                }
            }
        }
    } else if (key_code == KEY_C) {
        RH_TRACE("Collision sector:\n         "
                 "x: %d->%d\n         "
                 "y: %d->%d\n         "
                 "z: %d->%d\n         ",
                 state->sector.x_min, state->sector.x_max,
                 state->sector.y_min, state->sector.y_max,
                 state->sector.z_min, state->sector.z_max);
#if 0
        collision_grid_cell cell = state->grid.cells[state->gx][state->gy][state->gz];

        RH_TRACE("[%d,%d,%d]\n         %d triangles", state->gx, state->gy, state->gz, cell.num_surfaces);
        for (uint32 n = 0; n < cell.num_surfaces; n++) {
            collision_triangle tri = state->grid.triangles[cell.surfaces[n]];
            RH_TRACE(" #%d [%f,%f,%f]\n        "
                     "     [%f,%f,%f]\n        "
                     "     [%f,%f,%f]\n", n, 
                     tri.v1.x, tri.v1.y, tri.v1.z,
                     tri.v2.x, tri.v2.y, tri.v2.z,
                     tri.v3.x, tri.v3.y, tri.v3.z);
        }
#endif
    } else if (key_code == KEY_P) {
        state->paused = !state->paused;
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

    // TODO: make sure memory is zeroed at this point

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
    //[TRACE]:   Triangle 54/954
    //[WARN]:  Triangle 54:
    //v1: (-6.7, 0.0, 5.7)
    //v2: (-2.0, 0.0, 7.0)
    //v3: (-4.0, 0.0, 6.0)
    //[TRACE]: Done!
    state->triangle.v1 = { -6.7f, 0.0f, 5.7f };
    state->triangle.v2 = { -2.0f, 0.0f, 7.0f };
    state->triangle.v3 = { -4.0f, 0.0f, 6.0f };
    laml::Vec3 origin = state->triangle.v1 + state->triangle.v2 + state->triangle.v3;
    origin = origin / 3.0f;

    collision_create_grid(&state->trans_arena, &state->grid, {25.0f, -0.0f, -5.0f}, 0.5f, 256, 16, 256);
    //collision_create_grid(&state->trans_arena, &state->grid, { 0.0f, 0.0f, 0.0f }, 0.5f, 64, 32, 64);
    //collision_create_grid(&state->trans_arena, &state->grid, { 0.0f, 0.0f, 0.0f }, 1.0f, 64, 32, 64);
    resource_load_mesh_file_for_level("Data/Models/level1.mesh", state->level_geom, &state->grid);
    //collision_grid_add_triangle(&state->trans_arena, &state->grid, state->triangle, true);
    //collision_grid_add_triangle(&state->trans_arena, &state->grid, state->triangle, false);

    collision_grid_finalize(&state->trans_arena, &state->grid);

    struct _vert {
        laml::Vec3 position;
        laml::Vec3 normal;
    };
    laml::Vec3 norm = laml::cross(state->triangle.v2 - state->triangle.v1, state->triangle.v3 - state->triangle.v1);
    _vert verts[] = {
                      {state->triangle.v1, norm},
                      {state->triangle.v2, norm},
                      {state->triangle.v3, norm}};
    uint32 inds[] = { 0, 1, 2 };
    const ShaderDataType attr[] = {ShaderDataType::Float3, ShaderDataType::Float3, ShaderDataType::None};
    renderer_create_mesh(&state->tri_geom, 3, verts, 3, inds, attr);

    state->player.position = {0.0f, 1.0f, 0.0f};
    state->player.orientation = {0.0f, 0.0f, 0.0f, 1.0f};
    //state->player.scale = 0.5f;
    state->player.scale = 1.0f;
    state->player.height = 2.0f;
    state->player.radius = 0.3f;
    laml::Vec3 world_up(0.0f, 1.0f, 0.0f);
    collision_create_capsule(&state->capsule, &state->capsule_geom, state->player.height, state->player.radius, world_up);
    //resource_load_mesh_file("Data/Models/dance.mesh", &state->capsule_geom, 0, 0, 0);

    state->gx = 12;
    state->gy = 2;
    state->gz = 23;
    state->theta = 0.0f;

    return true;
}

bool32 game_update_and_render(RohinApp* app, render_packet* packet, real32 delta_time) {
    game_state* state = (game_state*)(app->memory.PermanentStorage);

    if (input_is_key_down(KEY_T)) {
        static int delay = 0;
        delay++;
        if (delay == 4) {
            collision_grid_cell* cell = &state->grid.cells[state->gx][state->gy][state->gz];
            do {
                state->gz++;
                if (state->gz == state->grid.num_z) {
                    state->gz = 0;
                    state->gy++;
                    if (state->gy == state->grid.num_y) {
                        state->gy = 0;
                        state->gx++;
                        if (state->gx == state->grid.num_x) {
                            state->gx = 0;
                        }
                    }
                }
                cell = &state->grid.cells[state->gx][state->gy][state->gz];
            } while (cell->num_surfaces == 0);

            delay = 0;
        }
    }

    if (!state->paused) {
        state->theta += 45.0f*delta_time;
    }
    state->player.position.x = 5.0f * cos(state->theta*laml::constants::deg2rad<real32>);
    state->player.position.y = 0.1f;
    state->player.position.z = 5.0f * sin(state->theta*laml::constants::deg2rad<real32>);
    collision_grid_get_sector(&state->grid, &state->sector, &state->capsule, state->player.position);
    //collision_grid_get_sector(&state->grid, &state->sector, &state->capsule, state->debug_camera.position + laml::Vec3(0.0f, -1.0f, 0.0f));

    // simulate game state
    laml::Mat4 eye(1.0f);
    int32 mouse_dx, mouse_dy;
    input_get_raw_mouse_offset(&mouse_dx, &mouse_dy);
    real32 x_sens = 10.0f;
    real32 y_sens = 5.0f;
    state->debug_camera.yaw   -= x_sens*mouse_dx*delta_time;
    state->debug_camera.pitch -= y_sens*mouse_dy*delta_time;
    if (state->debug_camera.pitch > 85.0f) {
        state->debug_camera.pitch = 85.0f;
    } else if (state->debug_camera.pitch < -85.0f) {
        state->debug_camera.pitch = -85.0f;
    }
    if (state->debug_camera.yaw > 360.0f) {
        state->debug_camera.yaw -= 360.0f;
    } else if (state->debug_camera.yaw < 0.0f) {
        state->debug_camera.yaw += 360.0f;
    }

    // TODO: this is a silly way to go, why not just ypr->quat
    laml::Mat3 camera_rot;
    laml::transform::create_transform_rotation(camera_rot, state->debug_camera.yaw, state->debug_camera.pitch, 0.0f);
    state->debug_camera.orientation = laml::transform::quat_from_mat(camera_rot);

    laml::Vec3 right = camera_rot._cols[0];
    laml::Vec3 up = camera_rot._cols[1];
    laml::Vec3 forward = -camera_rot._cols[2];
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
    state->debug_camera.position = state->debug_camera.position + (speed * vel * delta_time);

    laml::Mat4 player_transform;
    laml::Vec3 player_scale(state->player.scale);
    laml::transform::create_transform(player_transform, state->player.orientation, state->player.position, player_scale);

    // ...

    // push all the render commands to the render_packet
    packet->num_commands = 2;
    packet->commands = PushArray(packet->arena, render_command, packet->num_commands);
    
    packet->commands[0].model_matrix = eye;
    packet->commands[0].geom = *state->level_geom;
    //packet->commands[0].geom = state->tri_geom;
    packet->commands[0].material_handle = 0;

    packet->commands[1].model_matrix = player_transform;
    packet->commands[1].geom = state->capsule_geom;
    packet->commands[1].material_handle = 1;
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
    packet->sector = state->sector;
#endif

    // calculate view-point
    packet->camera_pos = state->debug_camera.position;
    packet->camera_orientation = state->debug_camera.orientation;

    return true;
}

void game_on_resize(RohinApp* app, uint32 new_width, uint32 new_height) {
    RH_INFO("Game on resize [%dx%d].", new_width, new_height);
}

void game_shutdown(RohinApp* app) {
    RH_INFO("Game shutdown.");
}
