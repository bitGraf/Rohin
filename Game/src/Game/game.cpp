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

enum class Bloon_Type : int8 { // no more than 255 bloon types!
    NONE = 0,
    RED,
    BLUE,
    GREEN,
    YELLOW,
    PINK,
    BLACK,
    WHITE,
    PURPLE,
    LEAD,
    ZEBRA,
    RAINBOW,
    CERAMIC,
    NUM_BLOON_TYPES
};

static const char* bloon_names[Bloon_Type::NUM_BLOON_TYPES] = {
    "-",
    "Red",
    "Blue",
    "Green",
    "Yellow",
    "Pink",
    "Black",
    "White",
    "Purple",
    "Lead",
    "Zebra",
    "Rainbow",
    "Ceramic",
};

static const laml::Vec3 bloon_color[Bloon_Type::NUM_BLOON_TYPES] = {
    laml::Vec3(),
    laml::Vec3(1.0f, 0.0f, 0.0f),   // RED
    laml::Vec3(0.0f, 0.0f, 1.0f),   // BLUE
    laml::Vec3(0.0f, 1.0f, 0.0f),   // GREEN
    laml::Vec3(1.0f, 1.0f, 0.0f),   // YELLOW
    laml::Vec3(1.0f, 0.0f, 1.0f),   // PINK
    laml::Vec3(0.0f, 0.0f, 0.0f),   // BLACK
    laml::Vec3(1.0f, 1.0f, 1.0f),   // WHITE
    laml::Vec3(0.7f, 0.4f, 0.8f),   // PURPLE
    laml::Vec3(0.8f, 0.8f, 0.8f),   // LEAD
    laml::Vec3(1.0f, 1.0f, 1.0f),   // ZEBRA
    laml::Vec3(1.0f, 1.0f, 1.0f),   // RAINBOW
    laml::Vec3(0.8f, 0.5f, 0.1f),   // CERAMIC
};

// bloon tags
#define TAG_DEAD 0x8000

struct bloon {
    real32 speed;
    real32 position; // parameter along the path.

    uint16 rank; // total strength of bloon, including children
    uint16 tags; // bit-field of various tags like magic,camo,fortified,etc.

    Bloon_Type  type;
    Bloon_Type  child_type;   // bloon type to spawn on pop
    int8        health;       // health of the outer layer (usually 1)
    int8        num_children; // num of children to spawn on pop
}; // should be 16 bytes, no padding

bloon* spawn_bloon(bloon* bloon_arr, real32 spawn_position, Bloon_Type type) {
    bloon new_bloon;
    new_bloon.position = spawn_position;
    new_bloon.type = type;
    
    switch (type) {
        case Bloon_Type::RED:  {
            new_bloon.speed = 1.0f;
            new_bloon.rank = 1;
            new_bloon.tags = 0;
            new_bloon.health = 1;
            new_bloon.child_type = Bloon_Type::NONE;
            new_bloon.num_children = 0;
        } break;
        case Bloon_Type::BLUE: {
            new_bloon.speed = 1.4f;
            new_bloon.rank = 2;
            new_bloon.tags = 0;
            new_bloon.health = 1;
            new_bloon.child_type = Bloon_Type::RED;
            new_bloon.num_children = 1;
        } break;
        case Bloon_Type::GREEN: {
            new_bloon.speed = 1.8f;
            new_bloon.rank = 2;
            new_bloon.tags = 0;
            new_bloon.health = 1;
            new_bloon.child_type = Bloon_Type::BLUE;
            new_bloon.num_children = 1;
        } break;
        case Bloon_Type::YELLOW: {
            new_bloon.speed = 3.2f;
            new_bloon.rank = 2;
            new_bloon.tags = 0;
            new_bloon.health = 1;
            new_bloon.child_type = Bloon_Type::GREEN;
            new_bloon.num_children = 1;
        } break;
        case Bloon_Type::PINK: {
            new_bloon.speed = 3.5f;
            new_bloon.rank = 2;
            new_bloon.tags = 0;
            new_bloon.health = 1;
            new_bloon.child_type = Bloon_Type::YELLOW;
            new_bloon.num_children = 1;
        } break;
        case Bloon_Type::BLACK: {
            new_bloon.speed = 1.8f;
            new_bloon.rank = 2;
            new_bloon.tags = 0;
            new_bloon.health = 1;
            new_bloon.child_type = Bloon_Type::PINK;
            new_bloon.num_children = 2;
        } break;
        case Bloon_Type::WHITE: {
            new_bloon.speed = 2.0f;
            new_bloon.rank = 2;
            new_bloon.tags = 0;
            new_bloon.health = 1;
            new_bloon.child_type = Bloon_Type::PINK;
            new_bloon.num_children = 2;
        } break;
        case Bloon_Type::PURPLE: {
            new_bloon.speed = 3.0f;
            new_bloon.rank = 2;
            new_bloon.tags = 0;
            new_bloon.health = 1;
            new_bloon.child_type = Bloon_Type::PINK;
            new_bloon.num_children = 2;
        } break;
        case Bloon_Type::LEAD: {
            new_bloon.speed = 1.0f;
            new_bloon.rank = 2;
            new_bloon.tags = 0;
            new_bloon.health = 1;
            new_bloon.child_type = Bloon_Type::BLACK;
            new_bloon.num_children = 2;
        } break;
        case Bloon_Type::ZEBRA: {
            new_bloon.speed = 1.8f;
            new_bloon.rank = 2;
            new_bloon.tags = 0;
            new_bloon.health = 1;
            //new_bloon.child_type = Bloon_Type::WHITE_AND_BLACK;
            new_bloon.child_type = Bloon_Type::WHITE;
            new_bloon.num_children = 2;
        } break;
        case Bloon_Type::RAINBOW: {
            new_bloon.speed = 2.2f;
            new_bloon.rank = 2;
            new_bloon.tags = 0;
            new_bloon.health = 1;
            new_bloon.child_type = Bloon_Type::ZEBRA;
            new_bloon.num_children = 2;
        } break;
        case Bloon_Type::CERAMIC: {
            new_bloon.speed = 2.5f;
            new_bloon.rank = 2;
            new_bloon.tags = 0;
            new_bloon.health = 1;
            new_bloon.child_type = Bloon_Type::RAINBOW;
            new_bloon.num_children = 2;
        } break;
    };

    ArrayPushValue(bloon_arr, new_bloon);

    return ArrayPeek(bloon_arr);
}

struct game_state {
    memory_arena arena;

    // basic scene
    scene_3D scene;

    resource_static_mesh bloon_mesh;
    resource_static_mesh tower_mesh;

    bool32 debug_mode;
    real32 sun_yp[2];


    /////
    memory_arena bloon_arena;
    real32 level_time;
    real32 game_speed;
    bool paused;

    uint64 bloon_arena_size;
    bloon* bloons; // dynarray

    bloon_path path;

    real32 time_since_last_spawn;
    real32 spawn_rate;
    uint32 num_bloons_to_spawn;
    uint32 num_spawned;

    real32 kill_cd;
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
    ////////////////////////////////////////
    RH_WARN("bloon: %llu bytes", sizeof(bloon));
    RH_WARN(".speed:  %llu|%llu", sizeof(bloon::speed),        offsetof(bloon, speed));
    RH_WARN(".pos:    %llu|%llu", sizeof(bloon::position),     offsetof(bloon, position));
    RH_WARN(".rank:   %llu|%llu", sizeof(bloon::rank),         offsetof(bloon, rank));
    RH_WARN(".tags:   %llu|%llu", sizeof(bloon::tags),         offsetof(bloon, tags));
    RH_WARN(".type:   %llu|%llu", sizeof(bloon::type),         offsetof(bloon, type));
    RH_WARN(".child:  %llu|%llu", sizeof(bloon::child_type),   offsetof(bloon, child_type));
    RH_WARN(".health: %llu|%llu", sizeof(bloon::health),       offsetof(bloon, health));
    RH_WARN(".num:    %llu|%llu", sizeof(bloon::num_children), offsetof(bloon, num_children));

    RH_WARN("entity_static: %llu bytes", sizeof(entity_static));
    ///////////////////////////////////////

    memory_index offset = sizeof(game_state);
    CreateArena(&state->arena, memory->GameStorageSize-offset, (uint8*)(memory->GameStorage)+offset);

    state->debug_mode = false;

    // load bloon mesh. all bloon entities use this same mesh
    resource_load_static_mesh("Data/Models/bloon.mesh", &state->bloon_mesh);
    state->bloon_mesh.materials[0].RoughnessFactor = 0.0f;
    resource_load_static_mesh("Data/Models/tower.mesh", &state->tower_mesh);

    // allocate memory for bloons
    uint64 INITIAL_RESERVE = 50'000;
    state->bloon_arena_size = INITIAL_RESERVE * sizeof(bloon);
    uint8* bloon_arena_data = (uint8*)PushSize_(&state->arena, state->bloon_arena_size);
    CreateArena(&state->bloon_arena, state->bloon_arena_size, bloon_arena_data);
    state->bloons = CreateArray(&state->bloon_arena, bloon, INITIAL_RESERVE-2);

    create_scene(&state->scene, "basic_scene", &state->arena);
    ArrayReserve(state->scene.static_entities, INITIAL_RESERVE);

    // define basic scene
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

    // define bloon stats
    state->num_bloons_to_spawn = 30;
    state->num_spawned = 0;
    state->spawn_rate = 0.5f;
    state->time_since_last_spawn = 0.0f;
    state->game_speed = 3.0f;

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

    resource_static_mesh* mesh = PushStruct(&state->arena, resource_static_mesh);
    resource_load_static_mesh("Data/Models/plane.mesh", mesh);
    mesh->materials[0].RoughnessFactor = 0.95f; // todo: pull materials out of mesh file? make it a separate thing entirely?
    mesh->materials[0].MetallicFactor = 0.0f;
    entity_static* floor = create_static_entity(&state->scene, "floor", mesh);
    floor->color = laml::Vec3(1.0f);

    entity_static* ent = create_static_entity(&state->scene, "tower", &state->tower_mesh);
    ent->position = laml::Vec3(-4.5f, 0.0f, -4.5f);
    ent->color = laml::Vec3(1.0f);

    RH_INFO("Scene created. %d Static entities. %d Skinned entities.",
            GetArrayCount(state->scene.static_entities),
            GetArrayCount(state->scene.skinned_entities));

    RH_INFO("Game initialized");

    state->kill_cd = 0.0f;
    spawn_bloon(state->bloons, 0, Bloon_Type::CERAMIC);

    state->paused = true;
}

// Silly way right now! basically recursively spawn/damage bloons
void damage_bloon(bloon* bloons_arr, bloon* b, int8 dmg) {
    b->health -= dmg;

    // check if bloon is dead
    if (b->health <= 0) {
        int8 overkill = -b->health;

        RH_INFO("%s bloon popped!", bloon_names[(int8)b->type]);
        if (b->child_type != Bloon_Type::NONE) {
            RH_INFO("Spawning %d %s bloons!", b->num_children, bloon_names[(int8)b->child_type]);
            for (int8 c = 0; c < b->num_children; c++) {
                bloon* child = spawn_bloon(bloons_arr, b->position - 0.1f*c, b->child_type);
                damage_bloon(bloons_arr, child, overkill);
            }
        }

        b->tags |= TAG_DEAD;
    }
}

GAME_API GAME_UPDATE_FUNC(GameUpdate) {
    game_state* state = (game_state*)memory->GameStorage;
    if (!memory->IsInitialized) {
        init_game(state, memory);

        memory->IsInitialized = true;

        RH_INFO("------ Scene Initialized -----------------------");
    }
    if (state->paused)
        delta_time *= 0.0f;
    else
        delta_time *= state->game_speed;

    state->level_time += delta_time;
    state->time_since_last_spawn += delta_time;
    state->kill_cd += delta_time;

    // check if bloon needs to be spawned
    while ((state->num_spawned < state->num_bloons_to_spawn) && (state->time_since_last_spawn > state->spawn_rate)) {
        // spawn a balloon
        state->num_spawned++;
        state->time_since_last_spawn -= state->spawn_rate;

        Bloon_Type type;
        if (state->num_spawned > 15) {
            type = Bloon_Type::BLUE;
        } else {
            type = Bloon_Type::RED;
        }
        //RH_INFO("Spawning %s bloon %u/%u!", bloon_names[(int8)type], state->num_spawned, state->num_bloons_to_spawn);
        //spawn_bloon(state->bloons, 0.0f, type);
    }

    // update all bloons

    // 1. get total count of bloons. This will include bloons marked as popped.
    //    We cache this now, so even if we spawn new ones, they would be after
    //    all the existing bloons, so we wouldn't update them just yet anyway
    uint32 num_bloons = (uint32)GetArrayCount(state->bloons);
    ((uint64*)state->scene.static_entities)[-1] = 2; // set count to 2 (just the floor + tower!)

    // kill the strongest bloon
    uint32 strongest_idx  = 0;
    int8   strongest_type = 0;
    real32 strongest_pos  = 0.0f;
    for (uint32 n = 0; n < num_bloons; n++) {
        bloon* b = &state->bloons[n];
        if (b->tags & TAG_DEAD) continue;

        if ((int8)b->type >= strongest_type && b->position > strongest_pos) {
            strongest_type = (int8)b->type;
            strongest_idx = n;
            strongest_pos = b->position;
        }
    }

    uint32 num_alive = 0;
    for (uint32 n = 0; n < num_bloons; n++) {
        bloon* b = &state->bloons[n];
        if (b->tags & TAG_DEAD) continue;

        num_alive++;

        b->position += b->speed * delta_time;

        // tmp! do 1 damage after distance of 25 to blues
        if (b->position > 3.0f && n == strongest_idx && (int8)b->type == strongest_type && (state->kill_cd > 1.0f)) {
            damage_bloon(state->bloons, b, 2);
            state->kill_cd = 0.0f;
        }

        if (b->tags & TAG_DEAD) continue; // it might be dead now!

        entity_static* ent = create_static_entity(&state->scene, "-", &state->bloon_mesh);
        ent->scale = laml::Vec3(0.3f, 0.3f, 0.3f);
        ent->color = bloon_color[(int8)b->type];
        // todo: make entities instances of meshes intead of meshes themselves
        // todo: separate mesh and material?
        // todo: do this entire things differently.

        if (b->position > state->path.total_length) {
            // 'kill it'
            ent->position = laml::Vec3(100.0f, 0.0f, 0.0f);
            b->tags |= TAG_DEAD;

            // calculate bloon total and subtract from life
            RH_WARN("%s bloon leaked!", bloon_names[(int8)b->type]);
        } else {
            // really bad lookup of the entire path to find the two relevant
            // nodes. should cache these or something.
            uint32 idx = 0;
            real32 f = 0.0f;
            real32 pos = b->position;
            for (uint32 i = 0; i < (state->path.num_nodes - 1); i++) {
                real32 left  = state->path.path_distance[i];
                real32 right = state->path.path_distance[i+1];

                if (left <= pos && pos < right) {
                    idx = i;
                    f = (pos - left) / (right - left);
                    break;
                }
            }

            ent->position = (state->path.nodes[idx] * (1.0f-f)) + (state->path.nodes[idx+1] * f);
        }
    }

    ImGui::Begin("Count");
    ImGui::Text("num_bloons = %d", num_bloons);
    ImGui::Text("num_alive  = %d", num_alive);
    ImGui::End();

    // update the tower
    #if 0
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
    #endif

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
    } else if (key_code == KEY_P) {
        state->paused = !state->paused;
        RH_INFO("Game %s.", state->paused ? "paused" : "unpaused");
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