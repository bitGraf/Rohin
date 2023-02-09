#pragma once

#include "Engine/Renderer/Render_Types.h"

struct joint {
    static const int32 NullIndex = -1;

    int32 parent_index;
    laml::Mat4 local_matrix;
    laml::Mat4 inverse_model_matrix;
    laml::Mat4 final_transform;
};

struct joint_debug {
    real32 length;
    char* name;
    laml::Mat4 model_matrix;
};

struct skeleton {
    uint32 flag;
    uint32 num_joints;
    joint* joints;

    joint_debug* joints_debug;
};

struct animation {
    char* name;
};

struct material {
    texture_2D diffuse_map;
    texture_2D normal_map;
    texture_2D amr_map;
};

struct debug_geometry {
    struct debug_vertex {
        laml::Vec3 position;
        laml::Vec3 normal;
    };

    uint32 num_verts;
    uint32 num_inds;
    debug_vertex* vertices;
    uint32* indices;
};