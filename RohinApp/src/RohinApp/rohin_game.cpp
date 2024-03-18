#include "rohin_game.h"

#include <Engine/Core/Logger.h>
#include <Engine/Core/Timing.h>
#include <Engine/Core/Input.h>
#include <Engine/Core/Event.h>
#include <Engine/Core/String.h>
#include <Engine/Memory/MemoryUtils.h>
#include <Engine/Memory/Memory_Arena.h>
#include <Engine/Renderer/Renderer.h>
#include <Engine/Renderer/Render_Types.h>
#include <Engine/Resources/Resource_Manager.h>
#include <Engine/Resources/Filetype/anim_file_reader.h>
#include <Engine/Collision/Collision.h>
#include <Engine/Collision/Character_Controller.h>
#include <Engine/Animation/Animation.h>

#include <Engine/Platform/Platform.h>

#include <imgui/imgui.h>

// Game code interface
global_variable const char* libgame_filename = "libgame.dll";
global_variable const char* loaded_libgame_filename = "loaded_libgame.dll";
global_variable const char* lock_filename = "lock.tmp";

#include <Game/game.h>
struct game_code
{
    void* GameCodeDLL;
    uint64 DLLLastWriteTime;

    game_update_fcn* GameUpdate;
    
    bool32 IsValid;
};

GAME_UPDATE_FUNC(GameUpdateStub) {
    RH_WARN("STUB FUNCTION");
}
game_code LoadGameCode(const char* LibPath, const char* TempLibPath, const char* LockPath) {
    game_code result = {};
    result.GameUpdate = GameUpdateStub; // just in case

    file_info lock_info;
    if (!platform_get_file_attributes(LockPath, &lock_info)) {
        file_info info;
        if (!platform_get_file_attributes(LibPath, &info)) {
            RH_INFO("Failed to get file-info for '%s'!", LibPath);
            return result;
        }

        result.DLLLastWriteTime = info.last_write_time;

        if (platform_copy_file(LibPath, TempLibPath)) {
            result.GameCodeDLL = platform_load_shared_library(TempLibPath);
            if (result.GameCodeDLL) {
                char datestr[64];
                platform_filetime_to_systime(info.last_write_time, datestr, 64);
                RH_INFO("Succesfully loaded '%s' as '%s'!\n         DLL Filesize:  %llu Kb\n         DLL WriteTime: %s",
                        LibPath, TempLibPath, info.file_size / 1024, datestr);
                //RH_INFO("DLL Filesize:  %llu Kb", info.file_size/1024);
                //RH_INFO("DLL WriteTime: %llu", info.last_write_time);

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

struct player_state {
    laml::Vec3 position;
    laml::Quat orientation;

    real32 scale;

    real32 pitch;
    real32 yaw;

    real32 height;
    real32 radius;
};

struct rohin_game_state {
    memory_arena perm_arena;
    memory_arena trans_arena;
    memory_arena mesh_arena;

    game_code game;

    resource_static_mesh* level_mesh;
    resource_static_mesh* player_mesh;

    uint32 num_static_meshes;
    resource_static_mesh* static_meshes;
    uint32 num_skinned_meshes;
    resource_skinned_mesh* skinned_meshes;
    resource_animation* guy_idle_anim;
    resource_animation* guy_walk_anim;
    resource_animation* guy_jump_anim;
    animation_controller controller;
    real64 speed;

    player_state player;
    player_state debug_camera;

    render_material missing_material;

    //collision_grid grid;
    //collision_sphere collider;
    //render_geometry collider_geom;
    //collision_sector sector;

    // level
    //level_data level;
};

int32  NUM_X   = 11;
int32  NUM_Y   = 11;
real32 spacing = 2.5f;

bool32 on_key_event(uint16 code, void* sender, void* listener, event_context context);
bool32 send_key_press_event_to_controller(uint16 code, void* sender, void* listener, event_context context);
bool32 send_key_release_event_to_controller(uint16 code, void* sender, void* listener, event_context context);

bool32 game_startup(RohinApp* app) {
    RH_INFO("Game startup.");

    // TODO: make sure memory is zeroed at this point

    rohin_game_state* state = (rohin_game_state*)(app->memory.PermanentStorage);
    CreateArena(&state->perm_arena, app->memory.PermanentStorageSize, (uint8*)app->memory.PermanentStorage + sizeof(rohin_game_state));
    state->mesh_arena = CreateSubArena(&state->perm_arena, Megabytes(1));

    CreateArena(&state->trans_arena, app->memory.TransientStorageSize, (uint8*)app->memory.TransientStorage);

    // load game-code
    state->game = LoadGameCode(libgame_filename, loaded_libgame_filename, lock_filename);
    if (!state->game.IsValid) {
        return false;
    }

    state->level_mesh  = PushStruct(&state->mesh_arena, resource_static_mesh);
    state->player_mesh = PushStruct(&state->mesh_arena, resource_static_mesh);
    
    state->num_static_meshes = NUM_X * NUM_Y;
    state->static_meshes  = PushArray(&state->mesh_arena, resource_static_mesh,  state->num_static_meshes);
    state->num_skinned_meshes = 1;
    state->skinned_meshes = PushArray(&state->mesh_arena, resource_skinned_mesh, state->num_static_meshes);

    state->guy_idle_anim = PushArray(&state->mesh_arena, resource_animation, 1);
    state->guy_walk_anim = PushArray(&state->mesh_arena, resource_animation, 1);
    state->guy_jump_anim = PushArray(&state->mesh_arena, resource_animation, 1);

    app->memory.IsInitialized = true;

    event_register(EVENT_CODE_KEY_PRESSED, state, on_key_event);

    return true;
}

bool32 game_initialize(RohinApp* app) {
    RH_INFO("Game initialize.");

    rohin_game_state* state = (rohin_game_state*)(app->memory.PermanentStorage);
    //resource_load_mesh_file("Data/Models/chao_garden/collision_meshes/Cube.mesh", state->player_geom, 0, 0, 0);

    // load the level geometry into the collision grid
    //resource_load_level_file("Data/Models/chao_garden/garden.level", &state->grid, state->level_geom);
    //resource_load_level_file("Data/Models/chao_garden/garden.level", &state->level);
    //collision_create_grid(&state->trans_arena, &state->grid, {25.0f, -0.1f, -5.0f}, 0.5f, 256, 16, 256);
    //resource_load_mesh_file_for_level("Data/Models/garden.mesh", state->level_geom, &state->grid);
    //collision_grid_finalize(&state->trans_arena, &state->grid);
    resource_load_static_mesh("Data/Models/helmet.mesh", state->player_mesh);

    state->missing_material.DiffuseFactor = laml::Vec3(1.0f);
    state->missing_material.NormalScale = 1.0f;
    state->missing_material.AmbientStrength = 1.0f;
    state->missing_material.MetallicFactor = 0.0f;
    state->missing_material.RoughnessFactor = 0.0;
    state->missing_material.EmissiveFactor = laml::Vec3(0.0f);
    state->missing_material.flag = 3;
    resource_load_texture_file("Data/textures/checker.png", &state->missing_material.DiffuseTexture);

    // load skinned character
    resource_load_skinned_mesh("Data/Models/guy.mesh", &state->skinned_meshes[0]);
    resource_load_animation("Data/Animations/guy_idle.anim", state->guy_idle_anim);
    resource_load_animation("Data/Animations/guy_walk.anim", state->guy_walk_anim);
    resource_load_animation("Data/Animations/guy_jump.anim", state->guy_jump_anim);

    // Create animation_controller. define animation_graph with a few states
    const uint32 param_jump_idx    = 0; 
    const uint32 param_walking_idx = 1;
    state->controller = create_anim_controller(3, 2, &state->mesh_arena);
    event_register(EVENT_CODE_KEY_PRESSED,  &state->controller, send_key_press_event_to_controller);
    event_register(EVENT_CODE_KEY_RELEASED, &state->controller, send_key_release_event_to_controller);
    define_parameter(&state->controller, param_jump_idx,    param_type::PARAM_INT,   "space", param_mode::PARAM_KEY_EVENT, (void*)(KEY_SPACE));
    define_parameter(&state->controller, param_walking_idx, param_type::PARAM_FLOAT, "speed", param_mode::PARAM_POLL,      (void*)(&state->speed));

    const uint32 node_idle_idx = 0;
    const uint32 node_walk_idx = 1;
    const uint32 node_jump_idx = 2;
    anim_graph_node node_idle = create_node("<idle>", 2, state->guy_idle_anim, 0, &state->mesh_arena);
    define_connection_float(&node_idle, 0, node_walk_idx, param_walking_idx, trigger_type::TRIGGER_GT, 0.1);
    define_connection_int(&node_idle, 1, node_jump_idx, param_jump_idx,    trigger_type::TRIGGER_EQ, 1);
    state->controller.graph.nodes[node_idle_idx] = node_idle;

    anim_graph_node node_walk = create_node("<walk>", 2, state->guy_walk_anim, 0, &state->mesh_arena);
    define_connection_float(&node_walk, 0, node_idle_idx, param_walking_idx, trigger_type::TRIGGER_LEQ, 0.1);
    define_connection_int(&node_walk, 1, node_jump_idx, param_jump_idx,    trigger_type::TRIGGER_EQ,  1);
    state->controller.graph.nodes[node_walk_idx] = node_walk;

    anim_graph_node node_jump = create_node("<jump>", 1, state->guy_jump_anim, NODE_PLAY_FULL, &state->mesh_arena);
    define_connection_default(&node_jump, 0, node_idle_idx);
    //define_connection_float(&node_jump, 1, node_walk_idx, param_walking_idx, trigger_type::TRIGGER_GT, 0.1);
    state->controller.graph.nodes[node_jump_idx] = node_jump;

    validate_controller(&state->controller);

    // load spheres
    resource_load_static_mesh("Data/Models/sphere.mesh", &state->static_meshes[0]);
    for (uint32 n = 1; n < state->num_static_meshes; n++) {
        memory_copy(&state->static_meshes[n], &state->static_meshes[0], sizeof(resource_static_mesh));
    }

    //state->player.position = {-5.0f, 1.0f, 0.0f};
    state->player.position = {0.0f, 1.5f, 3.0f};
    state->player.orientation = {0.0f, 0.0f, 0.0f, 1.0f};
    state->player.scale = 1.0f;
    state->player.height = 1.9f;
    state->player.radius = 0.3f;
    laml::Vec3 world_up(0.0f, 1.0f, 0.0f);
    //collision_create_capsule(&state->collider, &state->collider_geom, state->player.height, state->player.radius, world_up);
    //collision_create_sphere(&state->collider, &state->collider_geom, state->player.radius);

    // Player Position: [-0.1f,1.0f, 2.2]
    // Player Position: [-0.1f,1.0f,-1.8]
    //[2.118260,1.000000,-0.278601]
    laml::Vec3 start(-2.0f,1.0f, 0.85f);
    laml::Vec3   end( 2.0f,1.0f,-0.85f);
    laml::Vec3 move_dir = laml::normalize(end - start);
    real32 move_dist = laml::length(end - start);
    laml::Vec3 new_position = start;

    #ifdef DO_COLLISION
    sweep_result hit_result;
    laml::Vec3 tri_normal;
    uint32 max_iters = 8;
    uint32 num_steps = 0;
    while(max_iters--) {
        num_steps++;
        if (sweep_sphere_triangles(1, state->grid.triangles, new_position, 0.3f, move_dir, move_dist, hit_result, tri_normal)) {
            // collides with something this frame
            new_position = new_position + (move_dir*(hit_result.distance - 0.001f));

            RH_TRACE("Step %d: moving %.3f to [%.3f,%.3f,%.3f]. %.3f remaining",
                     num_steps, hit_result.distance,
                     new_position.x, new_position.y, new_position.z,
                     move_dist-hit_result.distance);

            move_dist -= hit_result.distance;
            if (move_dist <= 1e-5f) break; // done!
            move_dir = move_dir - laml::dot(move_dir, tri_normal)*tri_normal;
            if (laml::length(move_dir) <= 0.00001f) break;
            move_dir = laml::normalize(move_dir);

            //state->player.position = new_position;
            //RH_TRACE(" #%d [%f,%f,%f]\n", hit_result.face_index, 
            //         hit_result.position.x, hit_result.position.y, hit_result.position.z);
        } else {
            new_position = new_position + (move_dir*move_dist);
            RH_TRACE("Step %d: moving %.3f to [%.3f,%.3f,%.3f]. unobstructed",
                     num_steps, move_dist,
                     new_position.x, new_position.y, new_position.z);
            break;
        }
    }
    state->player.position = new_position;

    #endif

    return true;
}

static game_update_fcn game_update;

bool32 game_update_and_render(RohinApp* app, render_packet* packet, real32 delta_time) {
    time_point update_start = start_timer();
    rohin_game_state* state = (rohin_game_state*)(app->memory.PermanentStorage);

    // check if game.dll is out of date
    file_info info;
    platform_get_file_attributes(libgame_filename, &info);
    if (info.last_write_time != state->game.DLLLastWriteTime) {
        RH_INFO("Detected change in '%s'!", libgame_filename);
        platform_sleep(250);
        UnloadGameCode(&state->game);
        state->game = LoadGameCode(libgame_filename, loaded_libgame_filename, lock_filename);
    }

    game_state gs;
    gs.value = 25;
    state->game.GameUpdate(&gs);

    // Create ImGui window
    ImGui::Begin("RohinGame");
    ImGui::Text("Window made by %s", __FILE__);
    ImGui::Text("  DeltaTime: %.3f ms",  delta_time*1000.0f);
    ImGui::Text("  Framerate: %.3f fps", 1.0f / delta_time);

    // simulate game state
    laml::Mat4 eye(1.0f);
    int32 mouse_dx, mouse_dy;
    input_get_raw_mouse_offset(&mouse_dx, &mouse_dy);

    mouse_dy = 0;
    mouse_dx = 0;

    real32 x_sens = 10.0f;
    real32 y_sens = 5.0f;
    state->player.yaw   -= x_sens*mouse_dx*delta_time;
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
    laml::Mat3 player_rot;
    laml::transform::create_transform_rotation(player_rot, state->player.yaw, 0.0f, 0.0f);
    //state->player.orientation = laml::transform::quat_from_mat(player_rot);
    state->player.orientation = laml::Quat(1.0f, 0.0f, 0.0f, 0.0f);

    laml::Vec3 right   =  player_rot._cols[0];
    laml::Vec3 up      =  player_rot._cols[1];
    laml::Vec3 forward = -player_rot._cols[2];
    laml::Vec3 move_dir(0.0f);
    real32 speed = input_is_key_down(KEY_LSHIFT) ? 10.0f : 2.5f;
    bool32 moving = false;
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
    //if (input_is_key_down(KEY_SPACE)) {
    //    move_dir = move_dir + up;
    //    moving = true;
    //} else if (input_is_key_down(KEY_LCONTROL)) {
    //    move_dir = move_dir - up;
    //    moving = true;
    //}
    if (moving) move_dir = laml::normalize(move_dir);
    real32 move_dist = speed * delta_time;
    laml::Vec3 new_position = state->player.position;
    state->speed = moving ? speed : 0.0;

    update_controller(&state->controller);

    #ifdef DO_COLLISION
    collision_grid_get_sector_capsule(&state->grid, &state->sector, new_position, new_position+(move_dir*move_dist), state->collider.radius);
    collision_sphere collider;
    collider.C = state->collider.C + new_position;
    //collision_capsule collider;
    //collider.A = state->collider.A + new_position;// + laml::Vec3(0.0f, 1.0f, 0.0f);
    //collider.B = state->collider.B + new_position;// + laml::Vec3(0.0f, 1.0f, 0.0f);
    collider.radius = state->collider.radius;

    uint32 num_tris;
    uint32 num_intersecting_tris = 0;
    uint32* triangle_indices = collision_get_unique_triangles(&state->grid, &state->sector, packet->arena, &num_tris);
    uint32* intersecting_triangle_indices = PushArray(packet->arena, uint32, num_tris);
    collision_triangle* triangles = PushArray(packet->arena, collision_triangle, num_tris);

    //real32 dist = 1e10;
    //laml::Vec3 move_dir = laml::normalize(vel);
    laml::Vec3 contact_point;
    for (uint32 n = 0; n < num_tris; n++) {
        uint32 tri_idx = triangle_indices[n];
        triangles[n] = state->grid.triangles[tri_idx];
        laml::Vec3 curr_contact_point;
        if (triangle_sphere_intersect(triangles[n], collider, curr_contact_point)) {
            intersecting_triangle_indices[num_intersecting_tris++] = tri_idx;
        }
    }

#if 0
	// This doesn't work rn... BUT, its solving a problem that doesnt exist... :)
	if (num_intersecting_tris > 0) {
		// in a bad state!!! (this shouldnt happen?)
		RH_WARN("Im inside geometry right now... pushing outside");

		for (uint32 n = 0; n < num_intersecting_tris; n++) {
			collision_triangle *bad_tri = &state->grid.triangles[intersecting_triangle_indices[n]];
			laml::Vec3 bad_tri_normal = laml::cross(bad_tri->v3 - bad_tri->v1, bad_tri->v2 - bad_tri->v1);
			bad_tri_normal = laml::normalize(bad_tri_normal);
			
			real32 pen_depth = collider.radius - laml::dot(new_position - bad_tri->v1, bad_tri_normal);

			RH_INFO("Triangle %d: %d [depth = %f]", n, intersecting_triangle_indices[n], pen_depth);
		}
	}
#endif

    //state->player.position = collision_move_sphere(state->player.position, new_position+(move_dir*move_dist), state->player.radius, triangles);

#if 1
    sweep_result hit_result;
    laml::Vec3 tri_normal;
    if (moving) {
        uint32 max_iters = 8;
        uint32 num_steps = 0;
        while(max_iters--) {
            if (sweep_sphere_triangles(num_tris, triangles, new_position, collider.radius+0.001f, move_dir, move_dist, hit_result, tri_normal)) {
                contact_point = hit_result.position;
                // collides with something this frame
				real32 step_move_dist = laml::clamp(hit_result.distance - 0.001f, 0.0f, 1.0f);
                new_position = new_position + (move_dir*step_move_dist);
                num_steps++;
    
                //RH_TRACE("Step %d: moving %.3f to [%.2f,%.2f,%.2f]. %.1f",
                //         num_steps, hit_result.distance,
                //         new_position.x, new_position.y, new_position.z);
    
                laml::Vec3 new_dir = move_dir - laml::dot(move_dir, tri_normal)*tri_normal;
                move_dist = (move_dist - hit_result.distance) * laml::dot(new_dir, move_dir);
                if (move_dist <= 1e-5f) break; // done!
                move_dir = new_dir;
                if (laml::length(move_dir) <= 0.00001f) break;
                move_dir = laml::normalize(move_dir);
    
                //state->player.position = new_position;
                //RH_TRACE(" #%d [%f,%f,%f]\n", hit_result.face_index, 
                //         hit_result.position.x, hit_result.position.y, hit_result.position.z);
            } else {
                new_position = new_position + (move_dir*move_dist);
                break;
            }
        }
    }
#else
    new_position = new_position + (move_dir*move_dist);
#endif
    #else
        //new_position = new_position + (move_dir*move_dist);
    #endif
    state->player.position = new_position;


    state->debug_camera.position = state->player.position - (forward * 0.0f) + (up * 0.0f);
    laml::Mat3 camera_rot;
    laml::transform::create_transform_rotation(camera_rot, state->player.yaw, state->debug_camera.pitch, 0.0f);
    state->debug_camera.orientation = laml::transform::quat_from_mat(camera_rot);

    laml::Mat4 player_transform;
    laml::Vec3 player_scale(state->player.scale);
    laml::transform::create_transform(player_transform, state->player.orientation, state->player.position, player_scale);

    laml::Mat4 capsule_transform;
    laml::transform::create_transform(capsule_transform, state->player.orientation, state->player.position);

    // ...

    // Generate render packet for this frame

    // 1. find the total number of render commands -> this is where some sort of culling/filtering would happen
    packet->num_commands = 0;
    //for (uint32 n = 0; n < state->num_static_meshes; n++) {
    //    packet->num_commands += state->static_meshes[n].num_primitives;
    //}
    for (uint32 n = 0; n < state->num_skinned_meshes; n++) {
        packet->num_commands += state->skinned_meshes[n].num_primitives;
    }
    // 1.5 find total number of skeletons
    packet->num_skeletons = state->num_skinned_meshes; // 1 skeleton per mesh

    // 2. allocate memory on the frame-arena to hold commands
    packet->commands  = PushArray(packet->arena, render_command,  packet->num_commands);
    packet->skeletons = PushArray(packet->arena, render_skeleton, packet->num_skeletons+1); // +1 so 0 is reserved as no skeleton
    
    uint32 command_idx  = 0;
    uint32 skeleton_idx = 1;

    // skinned character
    for (uint32 m = 0; m < state->num_skinned_meshes; m++) {
        const resource_skinned_mesh* mesh = &state->skinned_meshes[m];
        laml::Mat4 mesh_transform(1.0f);
        for (uint32 p = 0; p < mesh->num_primitives; p++) {
            render_command& cmd = packet->commands[command_idx];

            cmd.model_matrix = mesh_transform;
            cmd.geom         = mesh->primitives[p];
            cmd.material     = mesh->materials[p];
            cmd.skeleton_idx = skeleton_idx;

            command_idx++;
        }

        render_skeleton& skeleton = packet->skeletons[skeleton_idx];
        skeleton.num_bones = mesh->skeleton.num_bones;
        skeleton.bones = PushArray(packet->arena, laml::Mat4, skeleton.num_bones);

        ImGui::Begin("AnimGraph");
        ImGui::Text("node_time = %.3f", state->controller.node_time);
        ImGui::Text("anim_time = %.3f", state->controller.anim_time);
        ImGui::SeparatorText("Params");
        for (uint32 n = 0; n < state->controller.graph.num_params; n++) {
            const anim_graph_param& param = state->controller.graph.params[n];

            switch (param.type) {
                case param_type::PARAM_INT:   {
                    ImGui::Text("%-8s: [ int ] %d", param.name, param.curr_value.as_int);
                } break;
                case param_type::PARAM_FLOAT: {
                    ImGui::Text("%-8s: [float] %f", param.name, param.curr_value.as_float);
                } break;
            }
        }
        ImGui::SeparatorText("Nodes");
        for (uint32 n = 0; n < state->controller.graph.num_nodes; n++) {
            const anim_graph_node& node = state->controller.graph.nodes[n];

            if (n == state->controller.current_node) {
                ImGui::Text(" * %s:'%s' - %d connections", node.name, node.anim->name, node.num_connections);
            } else {
                ImGui::Text("   %s:'%s' - %d connections", node.name, node.anim->name, node.num_connections);
            }
            for (uint32 c = 0; c < node.num_connections; c++) {
                anim_graph_connection& con = node.connections[c];
                anim_graph_param& param = state->controller.graph.params[con.param];

                if (con.trigger_type == trigger_type::TRIGGER_ALWAYS) {
                    ImGui::Text("     %d: -> '%s' by default", c, state->controller.graph.nodes[con.node].name);
                } else {

                    char* comp = "x";
                    switch (con.trigger_type) {
                        case trigger_type::TRIGGER_EQ: { comp = " ="; } break;
                        case trigger_type::TRIGGER_NEQ: { comp = "!="; } break;
                        case trigger_type::TRIGGER_LT: { comp = " <"; } break;
                        case trigger_type::TRIGGER_LEQ: { comp = "<="; } break;
                        case trigger_type::TRIGGER_GT: { comp = " >"; } break;
                        case trigger_type::TRIGGER_GEQ: { comp = ">="; } break;
                    }
                    char buffer[256] = { 0 };
                    switch (param.type) {
                        case param_type::PARAM_INT: {
                            string_build(buffer, 256, "%s %s %d", param.name, comp, con.trigger.as_int);
                        } break;
                        case param_type::PARAM_FLOAT: {
                            string_build(buffer, 256, "%s %s %.2f", param.name, comp, con.trigger.as_int);
                        } break;
                    }
                    ImGui::Text("     %d: -> '%s' if (%s)", c, state->controller.graph.nodes[con.node].name, buffer);
                }
            }
        }

        ImGui::End();

        anim_graph_node* curr_node = &state->controller.graph.nodes[state->controller.current_node];
        sample_animation_at_time((const resource_skinned_mesh*)mesh, (const resource_animation*)curr_node->anim, state->controller.node_time, skeleton.bones);

        skeleton_idx++;
    }
    state->controller.node_time += packet->delta_time;

    // spheres
    #if 0
    for (int32 m1 = 0; m1 < NUM_X; m1++) {
        real32 xpos = (m1-((NUM_X-1)/2)) * spacing;
        real32 roughness = 0.0f;
        if (NUM_X > 1)
            roughness = (m1 * 1.0f/(NUM_X-1));
        if (roughness < 0.05f) roughness = 0.05f;
        for (int32 m2 = 0; m2 < NUM_Y; m2++) {
            real32 ypos = (m2-((NUM_Y-1)/2)) * spacing;
            real32 metalness = 1.0f;
            if (NUM_Y > 1)
                metalness = m2 * 1.0f/(NUM_Y-1);

            int32 m = m1*NUM_Y + m2;

            laml::Vec3 pos(xpos, ypos, -2.0f);
            laml::Mat4 model(1.0f);
            laml::transform::create_transform_translate(model, pos);
            for (uint32 n = 0; n < state->static_meshes[m].num_primitives; n++) {
                render_command& cmd = packet->commands[command_idx];

                cmd.model_matrix = model;
                cmd.geom = state->static_meshes[m].primitives[n];
                cmd.material = state->static_meshes[m].materials[n];

                cmd.material.MetallicFactor = metalness;
                cmd.material.RoughnessFactor = roughness;
                cmd.material.DiffuseFactor = laml::Vec3(1.0f, 0.1f, 0.1f);

                cmd.skeleton_idx = 0;

                command_idx++;
            }
        }
    }
    #endif

    //packet->commands[0].model_matrix = eye;
    //packet->commands[0].geom = *state->level_geom;
    ////packet->commands[0].geom = state->tri_geom;
    //packet->commands[0].material_handle = 0;

    //packet->commands[1].model_matrix = player_transform;
    //packet->commands[1].geom = *state->player_geom;
    //packet->commands[1].material_handle = 1;
#if 0
    packet->num_commands = state->num_geometry;
    packet->commands = PushArray(packet->arena, render_command, packet->num_commands);
    for (uint32 n = 0; n < packet->num_commands; n++) {
        packet->commands[n].model_matrix = transform;
        packet->commands[n].geom = state->geometry[n];
        packet->commands[n].material = 0;
    }
#endif

#if 0
    packet->col_grid = &state->grid;
    packet->collider_geom.model_matrix = capsule_transform;
    packet->collider_geom.geom = state->collider_geom;
    packet->collider_geom.material = 0;

    packet->sector = state->sector;
    packet->num_tris = num_tris;
    packet->triangle_indices = triangle_indices;

    packet->num_intersecting_tris = num_intersecting_tris;
    packet->intersecting_triangle_indices = intersecting_triangle_indices;

    packet->contact_point = contact_point;
#endif

    // calculate view-point
    packet->camera_pos = state->debug_camera.position;
    packet->camera_orientation = state->debug_camera.orientation;

    // timing
    ImGui::Text("Game: Update and Render - %.3f ms", measure_elapsed_time(update_start)*1000.0f);
    ImGui::End();

    return true;
}

void game_on_resize(RohinApp* app, uint32 new_width, uint32 new_height) {
    RH_INFO("Game on resize [%dx%d].", new_width, new_height);
}

void game_shutdown(RohinApp* app) {
    rohin_game_state* state = (rohin_game_state*)(app->memory.PermanentStorage);

    UnloadGameCode(&state->game);
    RH_INFO("Game shutdown.");
}


bool32 on_key_event(uint16 code, void* sender, void* listener, event_context context) {
    rohin_game_state* state = (rohin_game_state*)listener;

    uint16 key_code = context.u16[0];
    if (key_code == KEY_C) {
        #if 0
        RH_TRACE("Collision sector:\n         "
                 "x: %d->%d\n         "
                 "y: %d->%d\n         "
                 "z: %d->%d\n         ",
                 state->sector.x_min, state->sector.x_max,
                 state->sector.y_min, state->sector.y_max,
                 state->sector.z_min, state->sector.z_max);
        collision_grid_cell cell = state->grid.cells[state->sector.x_min][state->sector.y_min][state->sector.z_min];

        RH_TRACE("[%d,%d,%d]\n         %d triangles", state->sector.x_min, state->sector.y_min, state->sector.z_min, cell.num_surfaces);
        for (uint32 n = 0; n < cell.num_surfaces; n++) {
            collision_triangle tri = state->grid.triangles[cell.surfaces[n]];
            RH_TRACE(" #%d [%f,%f,%f]\n        "
                     "     [%f,%f,%f]\n        "
                     "     [%f,%f,%f]\n", cell.surfaces[n], 
                     tri.v1.x, tri.v1.y, tri.v1.z,
                     tri.v2.x, tri.v2.y, tri.v2.z,
                     tri.v3.x, tri.v3.y, tri.v3.z);
        }
        #endif
    } else if (key_code == KEY_P) {
        RH_TRACE("Player Position: [%f,%f,%f]", 
                 state->player.position.x, state->player.position.y, state->player.position.z);
    } else if (key_code == KEY_R) {
        //state->player.position = {-5.0f, 1.0f, 0.0f};
        state->player.position = {0.0f, 1.0f, 0.0f};
        state->player.orientation = {0.0f, 0.0f, 0.0f, 1.0f};
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

bool32 send_key_press_event_to_controller(uint16 code, void* sender, void* listener, event_context context) {
    animation_controller* controller = (animation_controller*)listener;
    uint16 key_code = context.u16[0];

    controller_on_key_event(controller, key_code, true);

    return false;
}
bool32 send_key_release_event_to_controller(uint16 code, void* sender, void* listener, event_context context) {
    animation_controller* controller = (animation_controller*)listener;
    uint16 key_code = context.u16[0];

    controller_on_key_event(controller, key_code, false);

    return false;
}