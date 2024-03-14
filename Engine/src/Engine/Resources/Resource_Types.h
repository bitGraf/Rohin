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
    render_texture_2D diffuse_map;
    render_texture_2D normal_map;
    render_texture_2D amr_map;
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

enum class mesh_file_result : int32 {
    error = -1,
    is_static,
    is_skinned
};

struct resource_static_mesh {
    uint32 num_primitives;
    render_geometry *primitives;
    render_material *materials;

    laml::Mat4 transform;
};
struct resource_bone {
    uint32 bone_idx;
    int32  parent_idx;
    laml::Mat4 local_matrix;
    laml::Mat4 inv_model_matrix;

    real32 debug_length;
    char* debug_name;
};
struct resource_skeleton {
    uint32 num_bones;
    resource_bone* bones;
};
struct resource_skinned_mesh {
    uint32 num_primitives;
    render_geometry *primitives;
    render_material *materials;
    resource_skeleton skeleton;

    laml::Mat4 transform;
};

struct level_data {
    uint32 num_geometry;
    uint32 num_colliders;

    render_geometry *geometry;
    laml::Mat4 *geo_transforms;

    render_geometry *colliders;
    laml::Mat4 *collider_transforms;

    collision_grid grid;
};