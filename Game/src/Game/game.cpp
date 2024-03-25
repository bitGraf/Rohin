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

struct game_state {
    memory_arena arena;

    // basic scene
    scene_3D scene;
    entity_static  *static_entity;
    entity_skinned *skinned_entity;

    resource_static_mesh static_mesh;
    // character
    resource_skinned_mesh guy_mesh;
    resource_animation guy_idle_anim;
    resource_animation guy_walk_anim;
    resource_animation guy_run_anim;
    resource_animation guy_jump_anim;
    animation_controller guy_controller;
    laml::Vec3 position;
    laml::Vec3 velocity;
    laml::Quat orientation;
    real32 max_speed;
    real32 acc, stop_acc;
    real32 curr_speed;

    bool32 debug_mode;
    char scene_filename[256];
    char* saved_str1;
    char* saved_str2;
};

bool32 controller_key_press(uint16 code, void* sender, void* listener, event_context context);
bool32 controller_key_release(uint16 code, void* sender, void* listener, event_context context);

void init_game(game_state* state, game_memory* memory) {
    memory_index offset = sizeof(game_state);
    CreateArena(&state->arena, memory->GameStorageSize-offset, (uint8*)(memory->GameStorage)+offset);

    // load skinned character
    resource_load_skinned_mesh("Data/Models/guy.mesh",       &state->guy_mesh);
    resource_load_animation("Data/Animations/guy_idle.anim", &state->guy_idle_anim);
    resource_load_animation("Data/Animations/guy_walk.anim", &state->guy_walk_anim);
    resource_load_animation("Data/Animations/guy_walk.anim", &state->guy_run_anim);
    resource_load_animation("Data/Animations/guy_jump.anim", &state->guy_jump_anim);

    // Create animation_controller. define animation_graph with a few states
    const uint32 param_jump_idx  = 0; 
    const uint32 param_speed_idx = 1;
    state->guy_controller = create_anim_controller(4, 2, &state->arena);
    event_register(EVENT_CODE_KEY_PRESSED,  &state->guy_controller, controller_key_press);
    event_register(EVENT_CODE_KEY_RELEASED, &state->guy_controller, controller_key_release);
    define_parameter(&state->guy_controller, param_jump_idx,    param_type::PARAM_INT, "space", param_mode::PARAM_KEY_EVENT, (void*)(KEY_F));
    define_parameter(&state->guy_controller, param_speed_idx, param_type::PARAM_FLOAT, "speed", param_mode::PARAM_POLL,      (void*)(&state->curr_speed));

    const uint32 node_idle_idx = 0;
    const uint32 node_walk_idx = 1;
    const uint32 node_run_idx = 2;
    const uint32 node_jump_idx = 3;
    anim_graph_node node_idle = create_node("<idle>", 2, &state->guy_idle_anim, 0, &state->arena);
    define_connection_float(&node_idle, 0, node_run_idx,  param_speed_idx, trigger_type::TRIGGER_GT, 2.0f);
    define_connection_float(&node_idle, 1, node_walk_idx, param_speed_idx, trigger_type::TRIGGER_GT, 0.1f);
    state->guy_controller.graph.nodes[node_idle_idx] = node_idle;

    anim_graph_node node_walk = create_node("<walk>", 2, &state->guy_walk_anim, 0, &state->arena);
    define_connection_float(&node_walk, 0, node_idle_idx, param_speed_idx, trigger_type::TRIGGER_LEQ, 0.1f);
    define_connection_float(&node_walk, 1, node_run_idx,  param_speed_idx, trigger_type::TRIGGER_GT,  2.0f);
    state->guy_controller.graph.nodes[node_walk_idx] = node_walk;

    anim_graph_node node_run = create_node("<run>", 2, &state->guy_run_anim, 0, &state->arena);
    define_connection_float(&node_run, 0, node_idle_idx, param_speed_idx, trigger_type::TRIGGER_LEQ, 0.1f);
    define_connection_float(&node_run, 1, node_walk_idx, param_speed_idx, trigger_type::TRIGGER_LEQ, 2.0f);
    state->guy_controller.graph.nodes[node_run_idx] = node_run;

    anim_graph_node node_jump = create_node("<jump>", 1, &state->guy_jump_anim, NODE_PLAY_FULL, &state->arena);
    define_connection_default(&node_jump, 0, node_idle_idx);
    //define_connection_float(&node_jump, 1, node_walk_idx, param_walking_idx, trigger_type::TRIGGER_GT, 0.1f);
    state->guy_controller.graph.nodes[node_jump_idx] = node_jump;

    validate_controller(&state->guy_controller);

    state->position = laml::Vec3(0.0f, 0.0f, 0.0f);
    state->velocity = laml::Vec3(0.0f, 0.0f, 0.0f);
    state->max_speed = 4.0f;
    state->curr_speed = 0.0f;
    state->acc = 4.0f;
    state->stop_acc = 10.0f;

    state->debug_mode = false;

    state->saved_str1 = copy_string_to_arena("test1", &state->arena);
    state->saved_str2 = copy_string_to_arena("test2", &state->arena);

    // define basic scene
    create_scene(&state->scene, "basic_scene", &state->arena);

    state->scene.sun.direction = laml::normalize(laml::Vec3(1.5f, -1.0f, 0.0f));
    state->scene.sun.enabled = true;
    state->scene.sun.strength = 20.0f;

    resource_load_env_map("Data/env_maps/newport_loft.hdr", &state->scene.sky.environment);
    //resource_load_env_map("Data/env_maps/metro_noord_1k.hdr", &state->scene.sky.environment);

    resource_load_static_mesh("Data/Models/helmet.mesh", &state->static_mesh);
    state->static_entity = create_static_entity(&state->scene, "static_entity", &state->static_mesh);
    state->static_entity->position = laml::Vec3(-2.0f, 1.5f, 0.0f);
    state->static_entity->orientation = laml::transform::quat_from_axis_angle(laml::Vec3(1.0f, 0.0f, 0.0f) , 180.0f);

    state->skinned_entity = create_skinned_entity(&state->scene, "skinned_entity", &state->guy_mesh, &state->guy_controller);
    resource_static_mesh* mesh = PushStruct(&state->arena, resource_static_mesh);
    resource_load_static_mesh("Data/Models/plane.mesh", mesh);
    mesh->materials[0].RoughnessFactor = 0.95f; // todo: pull materials out of mesh file? make it a separate thing entirely?
    mesh->materials[0].MetallicFactor = 0.0f;
    create_static_entity(&state->scene, "floor", mesh);

    RH_INFO("Scene created. %d Static entities. %d Skinned entities.",
            GetArrayCount(state->scene.static_entities),
            GetArrayCount(state->scene.skinned_entities));

    state->guy_run_anim.frame_rate = 60.0f;
    state->guy_run_anim.length = state->guy_run_anim.num_samples / 60.0f;

    string_build(state->scene_filename, 256, "Data\\Scenes\\out.scene");

    RH_INFO("Game initialized");
}

GAME_API GAME_UPDATE_FUNC(GameUpdate) {
    game_state* state = (game_state*)memory->GameStorage;
    if (!memory->IsInitialized) {
        init_game(state, memory);

        memory->IsInitialized = true;

        RH_INFO("------ Scene Initialized -----------------------");
    }

    state->static_entity->euler_ypr[0] += 10.0f * delta_time;
    state->static_entity->euler_ypr[1] += 20.0f * delta_time;
    state->static_entity->euler_ypr[2] += 50.0f * delta_time;
    state->static_entity->orientation = laml::transform::quat_from_ypr(
        state->static_entity->euler_ypr[0],
        state->static_entity->euler_ypr[1],
        state->static_entity->euler_ypr[2]);
    
    if (!state->debug_mode) {
        laml::Vec3 forward(0.0f, 0.0f, -1.0f);
        laml::Vec3   right(1.0f, 0.0f,  0.0f);
        laml::Vec3      up(0.0f, 1.0f,  0.0f);

        bool32 move_input = false;
        bool32 is_moving = (laml::length_sq(state->velocity) > 0.01f);
        laml::Vec3 move_dir(0.0f);
        if (input_is_key_down(KEY_W)) {
            move_dir = move_dir + forward;
            move_input = true;
            is_moving = true;
        }
        if (input_is_key_down(KEY_S)) {
            move_dir = move_dir - forward;
            move_input = true;
            is_moving = true;
        }
        if (input_is_key_down(KEY_A)) {
            move_dir = move_dir - right;
            move_input = true;
            is_moving = true;
        }
        if (input_is_key_down(KEY_D)) {
            move_dir = move_dir + right;
            move_input = true;
            is_moving = true;
        }

        if (move_input) {
            state->velocity = state->velocity + move_dir*state->acc*delta_time;
        } else if (is_moving) {
            state->velocity = state->velocity * (1 - state->stop_acc*delta_time);
        } else {
            state->velocity = laml::Vec3(0.0f, 0.0f, 0.0f);
        }
        real32 max_speed = input_is_key_down(KEY_LSHIFT) ? 0.25f*state->max_speed : state->max_speed;
        state->curr_speed = laml::length(state->velocity);
        if (state->curr_speed > max_speed) {
            state->velocity = laml::normalize(state->velocity)*max_speed;
            state->curr_speed = max_speed;
        }

        state->position = state->position + state->velocity*delta_time;
        state->skinned_entity->position = state->position;
    }


    update_controller(&state->guy_controller, delta_time);

    char label_name[64];
    ImGui::Begin("Scene");
    ImGui::InputText("filename:", state->scene_filename, 256);
    if (ImGui::Button("Write scene to disk")) {
        RH_INFO("Saving scene to '%s'", state->scene_filename);
        serialize_scene(state->scene_filename, &state->scene);
    }
    if (ImGui::Button("Reload Skybox")) {
        RH_INFO("Dumb button pressed :3");
        resource_load_env_map("Data/env_maps/metro_noord_1k.hdr", &state->scene.sky.environment);
    }
    ImGui::Text("Name: %s", state->scene.name);
    ImGui::SeparatorText("Lighting");
    
    if (ImGui::TreeNode("Skybox")) {
        ImGui::DragFloat("Contribution", &state->scene.sky.strength, 0.01f, 0.0f, 1.0f);
        ImGui::Checkbox("Draw Skybox", &state->scene.sky.draw_skybox);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Sun")) {
        ImGui::Checkbox("Enabled", &state->scene.sun.enabled);
        static real32 sun_yp[2] = { -90.0f, -45.0f };
    //if (ImGui::CollapsingHeader("Sun")) {
        ImGui::DragFloat("Strength", &state->scene.sun.strength, 0.1f, 0.0f, 25.0f);
        ImGui::DragFloat2("Direction", sun_yp, 0.5f, -180.0f, 180.0f);
        state->scene.sun.direction = laml::transform::dir_from_yp(sun_yp[0], sun_yp[1]);
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

    #if 0 // static strings don't work with dll reloading.
    ImGui::Begin(state->saved_str1);
    ImGui::Text(state->saved_str2);
    ImGui::End();
    #else
    ImGui::Begin("AnimGraph");
    ImGui::Text("node_time = %.3f", state->guy_controller.node_time);
    ImGui::Text("anim_time = %.3f", state->guy_controller.anim_time);
    ImGui::SeparatorText("Params");
    for (uint32 n = 0; n < state->guy_controller.graph.num_params; n++) {
        const anim_graph_param& param = state->guy_controller.graph.params[n];

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
    for (uint32 n = 0; n < state->guy_controller.graph.num_nodes; n++) {
        const anim_graph_node& node = state->guy_controller.graph.nodes[n];

        if (n == state->guy_controller.current_node) {
            ImGui::Text(" * %s:'%s' - %d connections", node.name, node.anim->name, node.num_connections);
        } else {
            ImGui::Text("   %s:'%s' - %d connections", node.name, node.anim->name, node.num_connections);
        }
        for (uint32 c = 0; c < node.num_connections; c++) {
            anim_graph_connection& con = node.connections[c];
            anim_graph_param& param = state->guy_controller.graph.params[con.param];

            if (con.trigger_type == trigger_type::TRIGGER_ALWAYS) {
                ImGui::Text("     %d: -> '%s' by default", c, state->guy_controller.graph.nodes[con.node].name);
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
                ImGui::Text("     %d: -> '%s' if (%s)", c, state->guy_controller.graph.nodes[con.node].name, buffer);
            }
        }
    }

    ImGui::End();
    #endif

    return &state->scene;
}

GAME_API GAME_KEY_EVENT_FUNC(GameKeyEvent) {
    game_state* state = (game_state*)memory->GameStorage;

    if (key_code == KEY_C) {
        state->debug_mode = !state->debug_mode;
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