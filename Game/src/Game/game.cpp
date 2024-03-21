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

#include <imgui/imgui.h>

struct game_state {
    memory_arena arena;

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

    RH_INFO("Game initialized");
}

GAME_API GAME_UPDATE_FUNC(GameUpdate) {
    game_state* state = (game_state*)memory->GameStorage;
    if (!memory->IsInitialized) {
        init_game(state, memory);

        memory->IsInitialized = true;
    }
    
    // tmp change
    state->guy_run_anim.frame_rate = 60.0f;
    state->guy_run_anim.length = state->guy_run_anim.num_samples / state->guy_run_anim.frame_rate;

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
    }


    update_controller(&state->guy_controller, delta_time);

    // generate render commands
    // 1. find the total number of render commands -> this is where some sort of culling/filtering would happen
    packet->num_commands = state->guy_mesh.num_primitives;
    // 1.5 find total number of skeletons
    packet->num_skeletons = 1;

    // 2. allocate memory on the frame-arena to hold commands
    packet->commands  = PushArray(packet->arena, render_command,  packet->num_commands);
    packet->skeletons = PushArray(packet->arena, render_skeleton, packet->num_skeletons+1); // +1 so 0 is reserved as no skeleton
    
    uint32 command_idx  = 0;
    uint32 skeleton_idx = 1;

    // skinned character
    const resource_skinned_mesh* mesh = &state->guy_mesh;
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

    anim_graph_node* curr_node = &state->guy_controller.graph.nodes[state->guy_controller.current_node];
    sample_animation_at_time((const resource_skinned_mesh*)mesh, (const resource_animation*)curr_node->anim, state->guy_controller.node_time, skeleton.bones);

    skeleton_idx++;
}

GAME_API GAME_KEY_EVENT_FUNC(GameKeyEvent) {
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