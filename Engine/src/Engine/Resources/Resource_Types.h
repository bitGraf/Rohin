#pragma once

#include "Engine/Renderer/Render_Types.h"

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

struct bone_anim {
    laml::Vec3* translation;
    laml::Quat* rotation;
    laml::Vec3* scale;
};
struct resource_animation {
    char* name;

    uint16 num_bones;
    uint16 num_samples;
    real32 frame_rate;
    real32 length;

    bone_anim* bones;
};