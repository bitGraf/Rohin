#include "Animation.h"

#include "Engine/Core/Asserts.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Memory/MemoryUtils.h"
#include "Engine/Core/Input.h"

bool32 sample_animation_at_time(const resource_skinned_mesh* mesh,  const resource_animation* anim, real32 time, laml::Mat4* model_matrices) {
    uint16 num_samples = anim->num_samples;
    while (time > anim->length) {
        time -= anim->length;
    }

    const real32 sample_time = 1.0f / anim->frame_rate;
    uint16 sample_idx = (uint16)(time / sample_time); // just get the nearest index

    // 1. First! calculate the local transform for each bone
    uint16 num_bones = anim->num_bones;
    for (uint16 n = 0; n < num_bones; n++) {
        const bone_anim& bone = anim->bones[n];

        // 1.a build a transform from the TRS of the bone. Right now, just do 'nearest' interpolation (i.e. None).
        //     In the future, interpolate b/w frames and also between animations.
        laml::Vec3 translation = bone.translation[sample_idx];
        laml::Quat rotation    = bone.rotation[sample_idx];
        laml::Vec3 scale       = bone.scale[sample_idx];

        laml::transform::create_transform(model_matrices[n], rotation, translation, scale);

        // 1.b For each model_matrix, calculate its actual model-space transform by using 
        //     the inv_bind_pose matrices and parent_idx
        //     Note: we can do this in the same for-loop as above, since the bones 
        //     are ordered such that a bones parent ALWAYS occurs earlier in the list.
        if (mesh->skeleton.bones[n].parent_idx == -1) {
            // do nothing, no parent
        } else {
            // pre-multiply by the parent's local_transform
            int32 parent_idx = mesh->skeleton.bones[n].parent_idx;
            model_matrices[n] = laml::mul(model_matrices[parent_idx], model_matrices[n]);
        }
    }

    // 2. Now, get the difference transform by post-multiplying by the inv_bind_pose.
    for (uint16 n = 0; n < num_bones; n++) {
        const laml::Mat4& inv_bind_pose = mesh->skeleton.bones[n].inv_model_matrix;

        model_matrices[n] = laml::mul(model_matrices[n], inv_bind_pose);
    }

    return true;
}


animation_controller create_anim_controller(uint32 num_nodes, uint32 num_params, memory_arena* arena) {
    animation_controller controller;
    controller.current_node = 0;
    controller.node_time = 0.0;
    controller.arena = arena;

    anim_graph& graph = controller.graph;

    graph.num_nodes = num_nodes;
    graph.nodes = PushArray(arena, anim_graph_node, num_nodes);
    memory_zero(graph.nodes, graph.num_nodes*sizeof(anim_graph_node));

    graph.num_params = num_params;
    graph.params = PushArray(arena, anim_graph_param, num_params);
    memory_zero(graph.params, graph.num_params*sizeof(anim_graph_param));

    return controller;
}
void define_parameter(animation_controller* controller, uint32 param_idx, 
                      param_type type, const char* name, 
                      param_mode mode, void* watch_value) {

    anim_graph_param& param = controller->graph.params[param_idx];
    param.name = name;
    param.type = type;
    param.mode = mode;

    switch (mode) {
        case param_mode::PARAM_POLL: {
            param.update.watch_ptr = watch_value;

            switch (type) {
                case param_type::PARAM_INT: {
                    int64 default_value = 0;
                    param.curr_value.as_int = default_value;
                } break;
                case param_type::PARAM_FLOAT: {
                    real64 default_value = 0.0;
                    param.curr_value.as_float = default_value;
                } break;
                default: { 
                    Assert(!"Invalid PARAM_TYPE set"); 
                } break;
            }
        } break;
        case param_mode::PARAM_KEY_EVENT: {
            param.update.key_code = (uint64)watch_value;
            param.type = param_type::PARAM_INT; // doesn't make sense for key_events to be floats
        } break;
        case param_mode::PARAM_NONE: {
            Assert(!"Invalid PARAM_MODE set"); 
        } break;
    }
}

anim_graph_node create_node(const char* name, uint32 num_connections, resource_animation* anim, uint32 flag, memory_arena* arena) {
    anim_graph_node node;
    node.name = name;
    node.num_connections = num_connections;
    node.anim = anim;
    node.connections = PushArray(arena, anim_graph_connection, num_connections);
    memory_zero(node.connections, num_connections*sizeof(anim_graph_connection));

    node.flag = flag;
    node.anim_length = anim->num_samples / anim->frame_rate;

    return node;
}

void define_connection_float(anim_graph_node* node, uint32 connection_idx, uint32 new_node_idx, uint32 param_idx, trigger_type type, real64 float_trigger) {
    anim_graph_connection& con = node->connections[connection_idx];
    con.node = new_node_idx;
    con.param = param_idx;
    con.trigger_type = type;
    con.trigger.as_float = float_trigger;
}
void define_connection_int(anim_graph_node* node, uint32 connection_idx, uint32 new_node_idx, uint32 param_idx, trigger_type type, int64 int_trigger) {
    anim_graph_connection& con = node->connections[connection_idx];
    con.node = new_node_idx;
    con.param = param_idx;
    con.trigger_type = type;
    con.trigger.as_int = int_trigger;
}
void define_connection_default(anim_graph_node* node, uint32 connection_idx, uint32 new_node_idx) {
    anim_graph_connection& con = node->connections[connection_idx];
    con.node = new_node_idx;
    con.trigger_type = trigger_type::TRIGGER_ALWAYS;
}

bool32 test_float(real64 value, real64 trigger, trigger_type type) {
    switch (type) {
        case trigger_type::TRIGGER_EQ:     return (value == trigger);
        case trigger_type::TRIGGER_NEQ:    return (value != trigger);
        case trigger_type::TRIGGER_GT:     return (value >  trigger);
        case trigger_type::TRIGGER_GEQ:    return (value >= trigger);
        case trigger_type::TRIGGER_LT:     return (value < trigger);
        case trigger_type::TRIGGER_LEQ:    return (value <= trigger);
        case trigger_type::TRIGGER_ALWAYS: return true;
    }

    return false;
}

bool32 test_int(int64 value, int64 trigger, trigger_type type) {
    switch (type) {
        case trigger_type::TRIGGER_EQ:     return (value == trigger);
        case trigger_type::TRIGGER_NEQ:    return (value != trigger);
        case trigger_type::TRIGGER_GT:     return (value >  trigger);
        case trigger_type::TRIGGER_GEQ:    return (value >= trigger);
        case trigger_type::TRIGGER_LT:     return (value < trigger);
        case trigger_type::TRIGGER_LEQ:    return (value <= trigger);
        case trigger_type::TRIGGER_ALWAYS: return true;
    }

    return false;
}

void update_controller(animation_controller* controller) {
    anim_graph& graph = controller->graph;
    anim_graph_node& current_node = graph.nodes[controller->current_node];

    controller->anim_time = controller->node_time;
    while (controller->anim_time > current_node.anim_length) {
        controller->anim_time -= current_node.anim_length;
    }

    // update parameters
    for (uint32 n = 0; n < graph.num_params; n++) {
        anim_graph_param& param = graph.params[n];

        switch (param.mode) {
            case param_mode::PARAM_POLL: {
                switch (param.type) {
                    case param_type::PARAM_INT: {
                        int64* val_ptr = (int64*)param.update.watch_ptr;
                        param.curr_value.as_int = *val_ptr;
                    } break;
                    case param_type::PARAM_FLOAT: {
                        real64* val_ptr = (real64*)param.update.watch_ptr;
                        param.curr_value.as_float = *val_ptr;
                    } break;
                }
            } break;
            case param_mode::PARAM_KEY_EVENT: {
            } break;
        }
    }

    // check connections on current node
    if (current_node.flag & NODE_PLAY_FULL && (controller->node_time < current_node.anim_length)) return;

    bool32 con_triggered = false;
    for (uint32 n = 0; n < current_node.num_connections; n++) {
        anim_graph_connection& con = current_node.connections[n];
        anim_graph_param& param = graph.params[con.param];

        // first check if its an always trigger
        switch (con.trigger_type) {
            case trigger_type::TRIGGER_ALWAYS: {
                // transition to new node
                con_triggered = true;

                uint32 new_node_idx = con.node;
                controller->current_node = new_node_idx;
                controller->node_time = 0.0;
                controller->anim_time = 0.0;
            } break;
        }

        if (con_triggered) break;

        switch (param.type) {
            case param_type::PARAM_INT: {
                if (test_float(param.curr_value.as_float, con.trigger.as_float, con.trigger_type)) {
                    // transition to new node
                    con_triggered = true;

                    uint32 new_node_idx = con.node;
                    controller->current_node = new_node_idx;
                    controller->node_time = 0.0;
                    controller->anim_time = 0.0;
                }
            } break;
            case param_type::PARAM_FLOAT: {
                if (test_int(param.curr_value.as_int, con.trigger.as_int, con.trigger_type)) {
                    // transition to new node
                    con_triggered = true;

                    uint32 new_node_idx = con.node;
                    controller->current_node = new_node_idx;
                    controller->node_time = 0.0;
                    controller->anim_time = 0.0;
                }
            } break;
        }

        if (con_triggered) break;
    }
}

void controller_on_key_event(animation_controller * controller, uint16 key_code, bool32 pressed) {
    anim_graph& graph = controller->graph;

    // update KEY_EVENT parameters
    for (uint32 n = 0; n < graph.num_params; n++) {
        anim_graph_param& param = graph.params[n];

        if (param.mode == param_mode::PARAM_KEY_EVENT) {
            uint16 watch_key = (uint16)param.update.key_code;
            if (key_code == watch_key) {
                param.curr_value.as_int = pressed ? 1 : 0;
            }
        }
    }
}


RHAPI bool32 validate_controller(animation_controller* controller) {
    const anim_graph& graph = controller->graph;

    // check all params are defined
    uint32 num_params = graph.num_params;
    Assert(num_params != 0 && "0 params defined");
    Assert(num_params < 1024 && "Too many params defined");
    Assert(graph.params != nullptr && "Param array not allocated");
    for (uint32 n = 0; n < num_params; n++) {
        const anim_graph_param& param = graph.params[n];

        Assert(param.type != param_type::PARAM_NONE && "PARAM_TYPE cannot be PARAM_NONE");
        Assert(param.mode != param_mode::PARAM_NONE && "PARAM_MODE cannot be PARAM_NONE");

        if (param.mode == param_mode::PARAM_POLL) {
            Assert(param.update.watch_ptr != nullptr && "Watching a void pointer");
        }
        Assert(param.name != nullptr && "Param name is not set");
    }

    // check all nodes are defineds
    uint32 num_nodes = graph.num_nodes;
    Assert(num_nodes != 0 && "0 nodes defined");
    Assert(num_nodes < 1024 && "Too many nodes defined");
    Assert(graph.nodes != nullptr && "Node array not allocated");
    for (uint32 n = 0; n < num_nodes; n++) {
        const anim_graph_node& node = graph.nodes[n];

        // check connections
        uint32 num_connections = node.num_connections;
        Assert(num_connections != 0    && "0 connections defined");
        Assert(num_connections <  1024 && "Too many connections defined");
        Assert(node.connections != nullptr && "Connections arra not allocated");

        for (uint32 c = 0; c < num_connections; c++) {
            const anim_graph_connection& con = node.connections[c];

            Assert(con.node  < graph.num_nodes  && "Connection referencing node that doesn't exist");
            Assert(con.param < graph.num_params && "Connection referencing param that doesn't exist");
            Assert(con.trigger_type != trigger_type::TRIGGER_NONE && "TRIGGER_TYPE cannot be TRIGGER_NONE");
        }

        Assert(node.anim_length > 0.0 && "Animation length must be non-zero");

        Assert(node.anim != nullptr && "Node anim is not set");
        Assert(node.name != nullptr && "Node name is not set");
    }

    return true;
}