#include "Game.h"

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

#include <imgui/imgui.h>

struct bloon_path {
    uint32 num_nodes;
    laml::Vec3* nodes;
    real32* path_distance;
    real32 total_length;
};

struct game_state {
    memory_arena arena;

    // basic scene
    scene_3D scene;

    bloon_path path;
    real32 level_time;
    real32 time_since_last_spawn;
    real32 spawn_rate;
    uint32 num_bloons_to_spawn;
    uint32 num_spawned;
    laml::Vec3 spawn_location;
    entity_static* bloons;

    resource_static_mesh bloon_mesh;
    resource_static_mesh tower_mesh;

    real32 bloon_speed;
    real32* bloon_pos;
    real32 game_speed;

    bool32 debug_mode;

    real32 sun_yp[2];
};

bool32 controller_key_press(uint16 code, void* sender, void* listener, event_context context);
bool32 controller_key_release(uint16 code, void* sender, void* listener, event_context context);

static const real32 height = 0.5f;
static laml::Vec3 path[] = {
    laml::Vec3(-13.358f,   height, -2.1896f),
    laml::Vec3( -7.1466f,  height, -2.0415f),
    laml::Vec3( -6.1836f,  height, -2.7964f),
    laml::Vec3( -6.2542f,  height, -5.5243f),
    laml::Vec3( -5.1489f,  height, -6.4884f),
    laml::Vec3( -1.1935f,  height, -6.5637f),
    laml::Vec3( -0.2058f,  height, -5.2468f),
    laml::Vec3( -0.29983f, height, -0.14371f),
    laml::Vec3( -0.62906f, height,  5.5002f),
    laml::Vec3( -1.4521f,  height,  6.2527f),
    laml::Vec3( -7.3312f,  height,  6.2998f),
    laml::Vec3( -8.0837f,  height,  6.7231f),
    laml::Vec3( -8.0367f,  height,  7.8519f),
    laml::Vec3( -7.4488f,  height,  8.6749f),
    laml::Vec3( -7.0843f,  height,  8.6044f),
    laml::Vec3(  8.0485f,  height,  7.8519f),
    laml::Vec3(  8.0955f,  height,  3.8306f),
    laml::Vec3(  6.9432f,  height,  2.4901f),
    laml::Vec3(  3.5098f,  height,  2.5372f),
    laml::Vec3(  2.6632f,  height,  1.9022f),
    laml::Vec3(  2.5221f,  height, -2.3778f),
    laml::Vec3(  3.3452f,  height, -3.2244f),
    laml::Vec3(  7.3665f,  height, -3.2479f),
    laml::Vec3(  8.2601f,  height, -3.8593f),
    laml::Vec3(  8.3072f,  height, -7.5984f),
    laml::Vec3(  7.4606f,  height, -8.445f),
    laml::Vec3(  1.8872f,  height, -8.3744f),
    laml::Vec3(  0.93713f, height, -9.1787f),
    laml::Vec3(  0.87598f, height, -15.0f),
};

void init_game(game_state* state, game_memory* memory) {
    memory_index offset = sizeof(game_state);
    CreateArena(&state->arena, memory->GameStorageSize-offset, (uint8*)(memory->GameStorage)+offset);

    state->debug_mode = false;

    // load bloon mesh. all bloon entities use this same mesh
    resource_load_static_mesh("Data/Models/bloon.mesh", &state->bloon_mesh);
    state->bloon_mesh.materials[0].RoughnessFactor = 0.0f;
    resource_load_static_mesh("Data/Models/tower.mesh", &state->tower_mesh);

    // define bloon stats
    state->num_bloons_to_spawn = 30;
    state->num_spawned = 0;
    state->spawn_rate = 0.5f;
    state->time_since_last_spawn = 0.0f;
    state->spawn_location = laml::Vec3(-10.0f, 0.5f, 0.0f);
    state->bloon_speed = 1.5f;
    state->game_speed = 1.0f;

    bloon_path bp;
    bp.nodes = path;
    bp.num_nodes = sizeof(path)/sizeof(path[0]);
    bp.path_distance = PushArray(&state->arena, real32, bp.num_nodes);
    bp.path_distance[0] = 0.0f;
    bp.total_length = 0.0f;
    for (uint32 n = 1; n < bp.num_nodes; n++) {
        real32 segment_length = laml::length(path[n] - path[n - 1]);
        bp.path_distance[n] = bp.total_length + segment_length;
        bp.total_length += segment_length;

        RH_INFO("  Segment %2u: %6.2f | %6.2f", n, segment_length, bp.path_distance[n]);
    }
    RH_INFO("Path has %u nodes, %u segments.", bp.num_nodes, bp.num_nodes - 1);
    RH_INFO("Total length: %.2f m", bp.total_length);
    state->path = bp;

    state->bloon_pos = PushArray(&state->arena, real32, state->num_bloons_to_spawn);
    memory_zero(state->bloon_pos, sizeof(real32)*state->num_bloons_to_spawn);

    // define basic scene
    create_scene(&state->scene, "basic_scene", &state->arena);

    state->scene.sun.direction = laml::normalize(laml::Vec3(1.5f, -1.0f, -1.0f));
    state->scene.sun.enabled = true;
    state->scene.sun.cast_shadow = true;
    state->scene.sun.strength = 20.0f;
    state->scene.sun.shadowmap_projection_size = 15.0f;
    state->scene.sun.shadowmap_projection_depth = 25.0f;
    state->scene.sun.dist_from_origin = 12.5f;

    laml::Mat4 rot(1.0f);
    laml::transform::lookAt(rot, laml::Vec3(0.0f), state->scene.sun.direction, laml::Vec3(0.0f, 1.0f, 0.0f));
    //laml::transform::create_transform_rotation(rot, state->debug_camera.orientation);
    real32 roll;
    laml::transform::decompose(rot, state->sun_yp[0], state->sun_yp[0], roll);

    resource_load_env_map("Data/env_maps/newport_loft.hdr", &state->scene.sky.environment);
    state->scene.sky.draw_skybox = false;

    ArrayReserve(state->scene.static_entities, state->num_bloons_to_spawn+1);

    resource_static_mesh* mesh = PushStruct(&state->arena, resource_static_mesh);
    resource_load_static_mesh("Data/Models/plane.mesh", mesh);
    mesh->materials[0].RoughnessFactor = 0.95f; // todo: pull materials out of mesh file? make it a separate thing entirely?
    mesh->materials[0].MetallicFactor = 0.0f;
    create_static_entity(&state->scene, "floor", mesh);

    entity_static* ent = create_static_entity(&state->scene, "tower", &state->tower_mesh);
    ent->position = laml::Vec3(-4.5f, 0.0f, -4.5f);

    RH_INFO("Scene created. %d Static entities. %d Skinned entities.",
            GetArrayCount(state->scene.static_entities),
            GetArrayCount(state->scene.skinned_entities));

    RH_INFO("Game initialized");
}

GAME_API GAME_UPDATE_FUNC(GameUpdate) {
    game_state* state = (game_state*)memory->GameStorage;
    if (!memory->IsInitialized) {
        init_game(state, memory);

        memory->IsInitialized = true;

        RH_INFO("------ Scene Initialized -----------------------");
    }
    delta_time *= state->game_speed;

    state->level_time += delta_time;
    state->time_since_last_spawn += delta_time;

    // check if bloon needs to be spawned
    while ((state->num_spawned < state->num_bloons_to_spawn) && (state->time_since_last_spawn > state->spawn_rate)) {
        // spawn a balloon
        state->num_spawned++;
        state->time_since_last_spawn -= state->spawn_rate;

        RH_INFO("Spawning bloon %u/%u!", state->num_spawned, state->num_bloons_to_spawn);

        entity_static* bloon = create_static_entity(&state->scene, "bloon", &state->bloon_mesh);
        bloon->position = state->path.nodes[0];
        bloon->scale = laml::Vec3(0.3f, 0.3f, 0.3f);
    }

    // update all bloons
    uint32 curr_num_entities = (uint32)GetArrayCount(state->scene.static_entities);
    uint32 bloon_idx = 0;
    for (uint32 n = 0; n < curr_num_entities; n++) {
        entity_static* bloon = &state->scene.static_entities[n];
        if (string_compare(bloon->name, "bloon") != 0) continue;

        state->bloon_pos[bloon_idx] += state->bloon_speed*delta_time;
        real32 pos = state->bloon_pos[bloon_idx];

        if (pos > state->path.total_length) {
            // 'kill it'
            bloon->position = laml::Vec3(100.0f, 0.0f, 0.0f);
        } else {
            uint32 idx = 0;
            real32 f = 0.0f;
            for (uint32 i = 0; i < (state->path.num_nodes - 1); i++) {
                real32 left  = state->path.path_distance[i];
                real32 right = state->path.path_distance[i+1];

                if (left <= pos && pos < right) {
                    idx = i;
                    f = (pos - left) / (right - left);
                    break;
                }
            }

            bloon->position = (state->path.nodes[idx] * (1.0f-f)) + (state->path.nodes[idx+1] * f);
        }

        bloon_idx++;
    }

    // update the tower
    for (uint32 n = 0; n < curr_num_entities; n++) {
        entity_static* tower = &state->scene.static_entities[n];
        if (string_compare(tower->name, "tower") != 0) continue;

        real32 closest_dist = 1e6;
        uint32 closest_idx = 0;
        bloon_idx = 0;
        for (uint32 m = 0; m < curr_num_entities; m++) {
            if (m == n) continue;
            entity_static* bloon = &state->scene.static_entities[m];
            if (string_compare(bloon->name, "bloon") != 0) continue;

            real32 dist = laml::length(bloon->position - tower->position);
            if (dist < closest_dist) {
                closest_dist = dist;
                closest_idx = m;
            }
            bloon_idx++;
        }

        real32 tower_range = 2.0f;
        if (closest_dist < tower_range) {
            entity_static* bloon = &state->scene.static_entities[closest_idx];
            laml::Mat4 tower_trans;
            laml::transform::lookAt(tower_trans, 
                                    laml::Vec3(tower->position.x, 0.0f, tower->position.z), 
                                    laml::Vec3(bloon->position.x, 0.0f, bloon->position.z),
                                    laml::Vec3(0.0f, 1.0f, 0.0f));
            laml::Mat3 tower_rot = laml::minor(tower_trans, 3, 3);
            laml::Mat3 offset;
            laml::transform::create_transform_rotation(offset, 180.0f, 0.0f, 0.0f);
            tower->orientation = laml::transform::quat_from_mat(laml::mul(offset, tower_rot));
        }
    }

    ImGui::Begin("Bloons");
    ImGui::Text("LevelTime:  %.2f", state->level_time);
    ImGui::Text("SpawnTimer: %.2f", state->time_since_last_spawn);
    ImGui::Text("Bloons: %u/%u", state->num_spawned, state->num_bloons_to_spawn);
    ImGui::SliderFloat("GameSpeed", &state->game_speed, 0.0f, 3.0f);
    ImGui::End();

    // Debug window
    char label_name[64];
    ImGui::Begin("Scene");
    ImGui::SeparatorText("Lighting");
    
    if (ImGui::TreeNode("Skybox")) {
        ImGui::DragFloat("Contribution", &state->scene.sky.strength, 0.01f, 0.0f, 1.0f);
        ImGui::Checkbox("Draw Skybox", &state->scene.sky.draw_skybox);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Sun")) {
        ImGui::Checkbox("Enabled", &state->scene.sun.enabled);
        ImGui::SameLine();
        ImGui::Checkbox("Cast Shadows", &state->scene.sun.cast_shadow);
    //if (ImGui::CollapsingHeader("Sun")) {
        ImGui::DragFloat("Strength", &state->scene.sun.strength, 0.1f, 0.0f, 25.0f);
        ImGui::DragFloat2("Direction", state->sun_yp, 0.5f, -180.0f, 180.0f);
        ImGui::DragFloat("OrthoWidth", &state->scene.sun.shadowmap_projection_size, 0.1f, 1.0f, 50.0f);
        ImGui::DragFloat("OrthoDepth", &state->scene.sun.shadowmap_projection_depth, 0.1f, 1.0f, 100.0f);
        ImGui::DragFloat("DistFromOrigin", &state->scene.sun.dist_from_origin, 0.1f, 1.0f, 100.0f);
        ImGui::DragFloat3("Origin", state->scene.sun.origin_point._data, 0.1f, -10.0f, 10.0f);
        state->scene.sun.direction = laml::transform::dir_from_yp(state->sun_yp[0], state->sun_yp[1]);
        ImGui::ColorPicker3("Color", state->scene.sun.color._data);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Point Lights")) {
        uint32 num_point_lights = (uint32)GetArrayCount(state->scene.pointlights);
        ImGui::Text("%u pointlights", num_point_lights);
        ImGui::SameLine();
        if (ImGui::Button("Add PointLight")) {
            ArrayAdd(state->scene.pointlights);
            scene_point_light* newlight = ArrayPeek(state->scene.pointlights);

            newlight->position = laml::Vec3(0.0f, 1.0f, 0.0f);
            newlight->color = laml::Vec3(1.0f, 1.0f, 1.0f);
            newlight->strength = 25.0f;
        }
        for (uint64 n = 0; n < num_point_lights; n++) {
            scene_point_light* pl = &state->scene.pointlights[n];
            string_build(label_name, 64, "pointlight #%u", n+1);
            if (ImGui::TreeNode(label_name)) {
                ImGui::Checkbox("Enabled", &pl->enabled);
                ImGui::DragFloat3("Position", pl->position._data, 0.01f, -FLT_MAX, FLT_MAX);
                ImGui::DragFloat3("Color",    pl->color._data,    0.01f,  0.0f, 1.0f);
                ImGui::DragFloat("Strength", &pl->strength,       0.5f,   0.0f, FLT_MAX);

                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Spot Lights")) {
        uint32 num_spot_lights = (uint32)GetArrayCount(state->scene.spotlights);
        ImGui::Text("%u spotlight", num_spot_lights);
        ImGui::SameLine();
        if (ImGui::Button("Add SpotLight")) {
            ArrayAdd(state->scene.spotlights);
            scene_spot_light* newlight = ArrayPeek(state->scene.spotlights);
            num_spot_lights = (uint32)GetArrayCount(state->scene.spotlights);

            newlight->position = laml::Vec3(0.0f, 1.0f, 0.0f);
            newlight->direction = laml::Vec3(0.0f, -1.0f, 0.0f);
            newlight->color = laml::Vec3(1.0f, 1.0f, 1.0f);
            newlight->strength = 25.0f;
            newlight->inner = 55.0f;
            newlight->outer = 65.0f;
        }
        for (uint64 n = 0; n < num_spot_lights; n++) {
            scene_spot_light* sl = &state->scene.spotlights[n];
            string_build(label_name, 64, "spotlight #%u", n+1);
            if (ImGui::TreeNode(label_name)) {
                ImGui::Checkbox("Enabled", &sl->enabled);
                ImGui::DragFloat3("Position",  sl->position._data,  0.01f, -FLT_MAX, FLT_MAX);
                ImGui::DragFloat3("Direction", sl->direction._data, 0.01f, -FLT_MAX, FLT_MAX);
                ImGui::DragFloat3("Color",     sl->color._data,     0.01f,  0.0f, 1.0f);
                ImGui::DragFloat("Strength",  &sl->strength,        0.5f,   0.0f, FLT_MAX);
                ImGui::DragFloat("Inner",     &sl->inner,           0.5f,   0.0f, sl->outer-0.5f);
                ImGui::DragFloat("Outer",     &sl->outer,           0.5f,   0.0f, 90.0f);
                if (sl->outer < sl->inner)
                    sl->inner = sl->outer;

                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    ImGui::SeparatorText("Static Entities");
    uint64 num_entities = GetArrayCount(state->scene.static_entities);
    ImGui::Text("%u static_entities", num_entities);
    for (uint64 n = 0; n < num_entities; n++) {
        entity_static* ent = &state->scene.static_entities[n];
        string_build(label_name, 64, "static_entity #%u [%s]", n, ent->name);
        if (ImGui::TreeNode(label_name)) {
            ImGui::DragFloat3("Position", ent->position._data,    0.01f, -FLT_MAX, FLT_MAX);
            ImGui::DragFloat3("Rotation", ent->euler_ypr, 0.5f, -180.0f, 180.0f);
            ent->orientation = laml::transform::quat_from_ypr(ent->euler_ypr[0], ent->euler_ypr[1], ent->euler_ypr[2]);
            ImGui::DragFloat3("Scale",    ent->scale._data,       0.01f, 0.01f, FLT_MAX);

            ImGui::TreePop();
        }
    }

    ImGui::SeparatorText("Skinned Entities");
    num_entities = GetArrayCount(state->scene.skinned_entities);
    ImGui::Text("%u skinned_entities", num_entities);
    for (uint64 n = 0; n < num_entities; n++) {
        entity_skinned* ent = &state->scene.skinned_entities[n];
        string_build(label_name, 64, "skinned_entity #%u [%s]", n, ent->name);
        if (ImGui::TreeNode(label_name)) {
            ImGui::DragFloat3("Position", ent->position._data,    0.01f, -FLT_MAX, FLT_MAX);
            ImGui::DragFloat3("Rotation", ent->euler_ypr, 0.5f, -180.0f, 180.0f);
            ent->orientation = laml::transform::quat_from_ypr(ent->euler_ypr[0], ent->euler_ypr[1], ent->euler_ypr[2]);
            ImGui::DragFloat3("Scale",    ent->scale._data,       0.01f, 0.01f, FLT_MAX);
            ImGui::Separator();
            ImGui::Text("Node: '%s'",       ent->controller->graph.nodes[ent->controller->current_node].name);
            ImGui::Text("Aimation: '%s'",   ent->controller->graph.nodes[ent->controller->current_node].anim->name);
            ImGui::Text("node_time = %.3f", ent->controller->node_time);
            ImGui::Text("anim_time = %.3f", ent->controller->anim_time);

            ImGui::TreePop();
        }
    }

    ImGui::End();

    return &state->scene;
}

GAME_API GAME_KEY_EVENT_FUNC(GameKeyEvent) {
    game_state* state = (game_state*)memory->GameStorage;

    if (key_code == KEY_C) {
        state->debug_mode = !state->debug_mode;
    } else if (key_code == KEY_BACKSPACE) {
        // reset state
        ((uint64*)state->scene.static_entities)[-1] = 2; // set count to 1 (just the floor + tower!)

        state->num_spawned = 0;
        state->level_time = 0.0f;
        state->time_since_last_spawn = 0.0f;
        for (uint32 n = 0; n < state->num_bloons_to_spawn; n++) {
            state->bloon_pos[n] = 0.0f;
        }
    }
}


bool32 controller_key_press(uint16 code, void* sender, void* listener, event_context context) {
    animation_controller* controller = (animation_controller*)listener;
    uint16 key_code = context.u16[0];

    controller_on_key_event(controller, key_code, true);

    return false;
}
bool32 controller_key_release(uint16 code, void* sender, void* listener, event_context context) {
    animation_controller* controller = (animation_controller*)listener;
    uint16 key_code = context.u16[0];

    controller_on_key_event(controller, key_code, false);

    return false;
}