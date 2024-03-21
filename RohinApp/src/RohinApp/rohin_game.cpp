#include "rohin_game.h"

#include <Engine/Core/Logger.h>
#include <Engine/Core/Timing.h>
#include <Engine/Core/Input.h>
#include <Engine/Core/Event.h>
#include <Engine/Core/String.h>
#include <Engine/Memory/Memory.h>
#include <Engine/Memory/Memory_Arena.h>
#include <Engine/Renderer/Renderer.h>
#include <Engine/Renderer/Render_Types.h>
#include <Engine/Resources/Resource_Manager.h>
#include <Engine/Resources/Filetype/anim_file_reader.h>
#include <Engine/Collision/Collision.h>
#include <Engine/Collision/Character_Controller.h>
#include <Engine/Animation/Animation.h>
#include <Engine/Scene/Scene.h>

#include <Engine/Platform/Platform.h>

#include <imgui/imgui.h>

// Game code interface
global_variable char libgame_filename[256];
global_variable char loaded_libgame_filename[256];
global_variable char lock_filename[256];

#define MAX_STATIC_MESHES 1024
#define MAX_SKINNED_MESHES 16

#include <Game/game.h>
struct game_code {
    void* GameCodeDLL;
    uint64 DLLLastWriteTime;

    game_update_fcn*     GameUpdate;
    game_key_event_fcn*  GameKeyEvent;
    
    bool32 IsValid;
};

GAME_UPDATE_FUNC(GameUpdateStub) {
    RH_WARN("STUB FUNCTION");
    return nullptr;
}
GAME_KEY_EVENT_FUNC(GameKeyEventStub) {
    RH_WARN("STUB FUNCTION");
}
game_code LoadGameCode(const char* FullLibPath, const char* FullTempLibPath, const char* FullLockPath) {
    game_code result = {};
    result.GameUpdate = GameUpdateStub; // just in case

    file_info lock_info;
    if (!platform_get_file_attributes(FullLockPath, &lock_info)) {
        file_info info;
        if (!platform_get_file_attributes(FullLibPath, &info)) {
            RH_INFO("Failed to get file-info for '%s'!", FullLibPath);
            return result;
        }

        result.DLLLastWriteTime = info.last_write_time;

        if (platform_copy_file(FullLibPath, FullTempLibPath)) {
            result.GameCodeDLL = platform_load_shared_library(FullTempLibPath);
            if (result.GameCodeDLL) {
                char datestr[64];
                platform_filetime_to_systime(info.last_write_time, datestr, 64);

                // trim the full path to jus tthe filename
                const char* LibPath = FullLibPath;
                for (const char* s = LibPath; *s; s++) {
                    if (*s == '\\')
                        LibPath = s+1;
                }
                const char* TempLibPath = FullTempLibPath;
                for (const char* s = TempLibPath; *s; s++) {
                    if (*s == '\\')
                        TempLibPath = s+1;
                }
                RH_INFO("Succesfully loaded '%s' as '%s'!\n         DLL Filesize:  %llu Kb\n         DLL WriteTime: %s",
                        LibPath, TempLibPath, info.file_size / 1024, datestr);

                // get function pointers
                game_update_fcn * func = (game_update_fcn*)platform_get_func_from_lib(result.GameCodeDLL, "GameUpdate");

                if (func) {
                    result.GameUpdate = func;
                    result.IsValid = true;
                } else {
                    result.GameUpdate = GameUpdateStub;
                }
            } else {
                RH_ERROR("Failed to load game code: LoadLibrary failed!");
            }
        } else {
            RH_ERROR("Failed to load game code: CopyFile failed!");
        }
    } else {
        RH_WARN("Failed to load game code: lock still in place!");
    }

    return result;
}
void UnloadGameCode(game_code* game) {
    RH_INFO("Unloaded game library!");
    if (game->GameCodeDLL) {
        platform_unload_shared_library(game->GameCodeDLL);
        game->GameCodeDLL = 0;
    }

    game->DLLLastWriteTime = 0;
    game->IsValid = false;
    game->GameUpdate = GameUpdateStub;
}

struct camera_state {
    laml::Vec3 position;
    laml::Quat orientation;

    real32 scale;

    real32 pitch;
    real32 yaw;
    real32 roll;
};

struct rohin_app_state {
    memory_arena app_arena;

    game_code game;
    game_memory memory;

    uint32 num_static_meshes;
    resource_static_mesh* static_meshes;
    uint32 num_skinned_meshes;
    resource_skinned_mesh* skinned_meshes;

    bool32       cam_static;
    camera_state debug_camera;

    scene_3D *current_scene;
};

bool32 on_key_event(uint16 code, void* sender, void* listener, event_context context);

bool32 game_startup(RohinApp* app) {
    RH_INFO("Game startup.");

    // TODO: make sure memory is zeroed at this point
    rohin_app_state* state = (rohin_app_state*)(app->memory.AppStorage);
    CreateArena(&state->app_arena, app->memory.AppStorageSize - sizeof(rohin_app_state), (uint8*)app->memory.AppStorage + sizeof(rohin_app_state));

    state->memory.GameStorageSize = app->memory.GameStorageSize;
    state->memory.GameStorage     = app->memory.GameStorage;

    // load game-code
    // get full paths
    platform_get_full_library_path(libgame_filename,        256, "libgame.dll");
    platform_get_full_library_path(loaded_libgame_filename, 256, "loaded_libgame.dll");
    platform_get_full_library_path(lock_filename,           256, "lock.tmp");
    state->game = LoadGameCode(libgame_filename, loaded_libgame_filename, lock_filename);
    if (!state->game.IsValid) {
        return false;
    }

    state->num_static_meshes = 0;
    state->static_meshes  = PushArray(&state->app_arena, resource_static_mesh,  MAX_STATIC_MESHES);
    state->num_skinned_meshes = 0;
    state->skinned_meshes = PushArray(&state->app_arena, resource_skinned_mesh, MAX_SKINNED_MESHES);

    app->memory.IsInitialized = true;

    event_register(EVENT_CODE_KEY_PRESSED, state, on_key_event);

    return true;
}

bool32 game_initialize(RohinApp* app) {
    RH_INFO("Game initialize.");

    rohin_app_state* state = (rohin_app_state*)(app->memory.AppStorage);

    state->cam_static = true;
    state->debug_camera.position    = laml::Vec3(0.0f, 2.0f, 3.0f);
    state->debug_camera.orientation = laml::Quat(0.0f, 0.0f, 0.0f, 1.0f);

    state->debug_camera.pitch = 0.0f;
    state->debug_camera.yaw   = 0.0f;
    state->debug_camera.roll  = 0.0f;

    return true;
}

bool32 game_update_and_render(RohinApp* app, render_packet* packet, real32 delta_time) {
    time_point update_start = start_timer();
    rohin_app_state* state = (rohin_app_state*)(app->memory.AppStorage);

    // check if game.dll is out of date
    file_info info;
    platform_get_file_attributes(libgame_filename, &info);
    if (info.last_write_time != state->game.DLLLastWriteTime) {
        RH_INFO("Detected change in '%s'!", libgame_filename);
        platform_sleep(250);
        UnloadGameCode(&state->game);
        state->game = LoadGameCode(libgame_filename, loaded_libgame_filename, lock_filename);
    }

    state->current_scene = state->game.GameUpdate(&state->memory, packet, delta_time);

    // Create ImGui window
    ImGui::Begin("RohinGame");
    ImGui::Text("Window made by %s", __FILE__);
    ImGui::Text("  DeltaTime: %.3f ms",  delta_time*1000.0f);
    ImGui::Text("  Framerate: %.3f fps", 1.0f / delta_time);

    // simulate game state
    laml::Mat4 eye(1.0f);
    int32 mouse_dx, mouse_dy;
    input_get_raw_mouse_offset(&mouse_dx, &mouse_dy);

    real32 x_sens = 10.0f;
    real32 y_sens = 5.0f;
    real32 roll_sens = 0.0f; // set to 0 for now! need to fix rotation matrix to be body-fixed (if thats what I want really)
    if (!state->cam_static) {
        state->debug_camera.yaw -= x_sens * mouse_dx*delta_time;
        state->debug_camera.pitch -= y_sens * mouse_dy*delta_time;
        if (input_is_key_down(KEY_E)) {
            state->debug_camera.roll += roll_sens * delta_time;
        }
        if (input_is_key_down(KEY_Q)) {
            state->debug_camera.roll -= roll_sens * delta_time;
        }
    }
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

    laml::Mat3 cam_rot(1.0f);
    laml::transform::create_transform_rotation(cam_rot, state->debug_camera.yaw, state->debug_camera.pitch, state->debug_camera.roll);

    laml::Vec3 right   =  cam_rot._cols[0];
    laml::Vec3 up      =  cam_rot._cols[1];
    laml::Vec3 forward = -cam_rot._cols[2];
    laml::Vec3 move_dir(0.0f);
    real32 speed = input_is_key_down(KEY_LSHIFT) ? 10.0f : 2.5f;
    bool32 moving = false;
    if (!state->cam_static) {
        if (input_is_key_down(KEY_W)) {
            move_dir = forward;
            moving = true;
        } else if (input_is_key_down(KEY_S)) {
            move_dir = -forward;
            moving = true;
        }
        if (input_is_key_down(KEY_D)) {
            move_dir = move_dir + right;
            moving = true;
        } else if (input_is_key_down(KEY_A)) {
            move_dir = move_dir - right;
            moving = true;
        }
        if (input_is_key_down(KEY_SPACE)) {
            move_dir = move_dir + up;
            moving = true;
        } else if (input_is_key_down(KEY_LCONTROL)) {
            move_dir = move_dir - up;
            moving = true;
        }
        if (moving) move_dir = laml::normalize(move_dir);
    }
    real32 move_dist = speed * delta_time;

    state->debug_camera.position = state->debug_camera.position + (move_dir*move_dist);
    state->debug_camera.orientation = laml::transform::quat_from_mat(cam_rot);

    // calculate view-point
    packet->camera_pos = state->debug_camera.position;
    packet->camera_orientation = state->debug_camera.orientation;

    // generate render commands
    if (state->current_scene) {
        // 1. find the total number of render commands -> this is where some sort of culling/filtering would happen
        uint32 num_static_meshes = (uint32)GetArrayCount(state->current_scene->static_entities);
        uint32 num_skinned_meshes = (uint32)GetArrayCount(state->current_scene->skinned_entities);

        packet->num_commands = 0;
        packet->num_skeletons = 0;
        for (uint32 n = 0; n < num_static_meshes; n++) {
            const resource_static_mesh* mesh = state->current_scene->static_entities[n].static_mesh;
            packet->num_commands += mesh->num_primitives;
        }
        for (uint32 n = 0; n < num_skinned_meshes; n++) {
            const resource_skinned_mesh* mesh = state->current_scene->skinned_entities[n].skinned_mesh;
            packet->num_commands += mesh->num_primitives;
            packet->num_skeletons++;
        }

        // 2. allocate memory on the frame-arena to hold commands
        packet->commands = PushArray(packet->arena, render_command,  packet->num_commands);
        packet->skeletons = PushArray(packet->arena, render_skeleton, packet->num_skeletons+1); // start at 1, so skeleton[0] is reserved
    
        uint32 command_idx = 0;
        uint32 skeleton_idx = 1; // 0 is reserved!

        // static meshes
        for (uint32 n = 0; n < num_static_meshes; n++) {
            const entity_static& entity = state->current_scene->static_entities[n];
            const resource_static_mesh* mesh = entity.static_mesh;
            mesh->transform; // use this?

            for (uint32 p = 0; p < mesh->num_primitives; p++) {
                render_command& cmd = packet->commands[command_idx];

                cmd.model_matrix = entity.transform; // or just this?
                cmd.geom = mesh->primitives[p];
                cmd.material = mesh->materials[p];
                cmd.skeleton_idx = 0;

                command_idx++;
            }
        }

        // skinned meshes
        for (uint32 n = 0; n < num_skinned_meshes; n++) {
            const entity_skinned& entity = state->current_scene->skinned_entities[n];
            const resource_skinned_mesh* mesh = entity.skinned_mesh;
            mesh->transform; // use this?

            for (uint32 p = 0; p < mesh->num_primitives; p++) {
                render_command& cmd = packet->commands[command_idx];

                cmd.model_matrix = entity.transform;
                cmd.geom = mesh->primitives[p];
                cmd.material = mesh->materials[p];
                cmd.skeleton_idx = skeleton_idx;

                command_idx++;
            }

            render_skeleton& skeleton = packet->skeletons[skeleton_idx];
            skeleton.num_bones = mesh->skeleton.num_bones;
            skeleton.bones = PushArray(packet->arena, laml::Mat4, skeleton.num_bones);

            const animation_controller* controller = entity.controller;
            const anim_graph_node& curr_node = controller->graph.nodes[controller->current_node];
            sample_animation_at_time(mesh, curr_node.anim, controller->node_time, skeleton.bones);

            skeleton_idx++;
        }
    }

    // timing
    ImGui::Text("Game: Update and Render - %.3f ms", measure_elapsed_time(update_start)*1000.0f);
    ImGui::End();

    return true;
}

void game_on_resize(RohinApp* app, uint32 new_width, uint32 new_height) {
    RH_INFO("Game on resize [%dx%d].", new_width, new_height);
}

void game_shutdown(RohinApp* app) {
    rohin_app_state* state = (rohin_app_state*)(app->memory.AppStorage);

    UnloadGameCode(&state->game);
    RH_INFO("Game shutdown.");
}


bool32 on_key_event(uint16 code, void* sender, void* listener, event_context context) {
    rohin_app_state* state = (rohin_app_state*)listener;

    uint16 key_code = context.u16[0];
    if (key_code == KEY_R) {
        RH_TRACE("Resetting camera position");
        state->debug_camera.position = {0.0f, 0.0f, 0.0f};
        state->debug_camera.orientation = {0.0f, 0.0f, 0.0f, 1.0f};
        state->debug_camera.pitch = 0.0f;
        state->debug_camera.yaw = 0.0f;
    }
    else if (key_code == KEY_P) {
        RH_TRACE("Position: [%.2f, %.2f, %.2f]\n         Orientation: [%.2f, %.2f, %.2f, %.2f]", 
                 state->debug_camera.position.x,
                 state->debug_camera.position.y,
                 state->debug_camera.position.z,
                 state->debug_camera.orientation.x,
                 state->debug_camera.orientation.y,
                 state->debug_camera.orientation.z,
                 state->debug_camera.orientation.w);
    }
    else if (key_code == KEY_C) {
        state->cam_static = !state->cam_static;
        RH_TRACE("Camera Mode: [%s]", state->cam_static ? "static" : "wasd+mouselook");
    }

    return false;
}