#pragma once

#include "Engine/Defines.h"
#include <laml/laml.hpp>

struct memory_arena;
struct triangle_geometry;

enum renderer_api_type {
    RENDERER_API_OPENGL,
};

enum class ShaderDataType : uint8 {
    None = 0, 
    Float, Float2, Float3, Float4,
    Mat3, Mat4, 
    Int, Int2, Int3, Int4,
    Bool
};

struct renderer_api {
    //struct platform_state* plat_state; // platform-specific state
    uint64 frame_number;

    virtual bool32 initialize(const char* application_name, struct platform_state* plat_state) = 0;
    virtual void shutdown() = 0;
    virtual void resized(uint16 width, uint16 height) = 0;

    virtual bool32 begin_frame(real32 delta_time) = 0;
    virtual bool32 end_frame(real32 delta_time) = 0;

    virtual void create_texture(struct texture_2D* texture, const uint8* data) = 0;
    virtual void destroy_texture(struct texture_2D* texture) = 0;
    virtual void create_mesh(triangle_geometry* mesh, 
                             uint32 num_verts, const void* vertices,
                             uint32 num_inds, const uint32* indices,
                             const ShaderDataType* attributes) = 0;
    virtual void destroy_mesh(triangle_geometry* mesh) = 0;
    virtual void shader_create(struct shader* shader_prog, const char* shader_source_filename) = 0;
    virtual void shader_destroy(struct shader* shader_prog) = 0;
};

struct render_command {
    laml::Mat4 model_matrix;
    uint32 mesh_handle;
    uint32 material_handle;
};

struct render_packet {
    memory_arena* arena;

    real32 delta_time;

    laml::Mat4 projection_matrix;
    laml::Mat4 view_matrix;
    laml::Vec3 camera_pos;

    uint32 num_commands;
    render_command* commands;
};

// TODO: not needed here
struct vertex_static {
    laml::Vec3 Position;
    laml::Vec3 Normal;
    laml::Vec3 Tangent;
    laml::Vec3 Bitangent;
    laml::Vec2 Texcoord;
};

struct vertex_anim {
    laml::Vec3 Position;
    laml::Vec3 Normal;
    laml::Vec3 Tangent;
    laml::Vec3 Bitangent;
    laml::Vec2 Texcoord;

    int32 BoneIndices[4];
    laml::Vec4 BoneWeights;
};

// the actual geometry that the gpu holds onto
// separately, a 'resource' will exist to 
// represent an actual mesh.
struct triangle_geometry {
    uint32 handle; // handle to the gpu version of this data

    uint32 num_verts;
    uint32 num_inds;
    uint32 flag;
};

struct texture_2D {
    uint32 handle;

    uint16 width;
    uint16 height;
    uint16 num_channels;
    uint16 flag;
};