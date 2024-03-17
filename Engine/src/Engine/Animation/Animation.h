#pragma once

#include "Engine/Resources/Resource_Types.h"

struct anim_var {

    union {
        int64  as_int;
        real64 as_float;
    } trigger;
};

enum class param_type : uint32 {
    PARAM_NONE = 0,
    PARAM_FLOAT,
    PARAM_INT,
};
enum class param_mode : uint32 {
    PARAM_NONE = 0,
    PARAM_POLL,
    PARAM_KEY_EVENT,
};
enum class trigger_type : uint32 {
    TRIGGER_NONE = 0,
    TRIGGER_EQ,
    TRIGGER_NEQ,
    TRIGGER_GT,
    TRIGGER_GEQ,
    TRIGGER_LT,
    TRIGGER_LEQ,
    TRIGGER_ALWAYS
};

#define NODE_PLAY_FULL 0x01

struct anim_graph_connection {
    uint32 node;
    uint32 param;

    trigger_type trigger_type;
    union {
        int64  as_int;
        real64 as_float;
    } trigger;
};

struct anim_graph_node {
    const char* name;

    uint32 num_connections;
    real32 anim_length;
    uint32 flag;
    anim_graph_connection* connections;

    resource_animation* anim;
};

struct anim_graph_param {
    const char* name;
    param_type type;
    param_mode mode;

    union {
        int64  as_int;
        real64 as_float;
    } curr_value;

    union {
        void* watch_ptr;
        uint64 key_code;
    } update;
};

struct anim_graph {
    uint32 num_nodes, num_params;
    anim_graph_node* nodes;
    anim_graph_param* params;
};

struct memory_arena;
struct animation_controller {
    uint32 current_node;
    real32 node_time;
    real32 anim_time;

    anim_graph graph;
    memory_arena* arena;
};

RHAPI bool32 sample_animation_at_time(const resource_skinned_mesh* mesh,  const resource_animation* anim, real32 time, laml::Mat4* model_matrices);

RHAPI animation_controller create_anim_controller(uint32 num_nodes, uint32 num_params, memory_arena* arena);
RHAPI void define_parameter(animation_controller* controller, uint32 param_idx, 
                            param_type type, const char* name, 
                            param_mode mode,  void* watch_value);
RHAPI anim_graph_node create_node(const char* name, uint32 num_connections, resource_animation* anim, uint32 flag, memory_arena* arena);
RHAPI void define_connection_float(anim_graph_node* node, uint32 connection_idx, uint32 new_node_idx, uint32 param_idx, trigger_type type, real64 float_trigger);
RHAPI void define_connection_int(anim_graph_node* node, uint32 connection_idx, uint32 new_node_idx, uint32 param_idx, trigger_type type, int64  int_trigger);
RHAPI void define_connection_default(anim_graph_node* node, uint32 connection_idx, uint32 new_node_idx);
RHAPI void update_controller(animation_controller* controller);
RHAPI void controller_on_key_event(animation_controller* controller, uint16 key_code, bool32 pressed);