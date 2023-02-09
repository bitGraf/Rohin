#pragma once

#include "Engine/Defines.h"
#include <laml/laml.hpp>

#include "Engine/Collision/Collision_Types.h"

struct memory_arena;
struct triangle_geometry;
struct shader;
struct frame_buffer;
struct frame_buffer_attachment;
struct collision_grid;

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

enum class render_draw_mode : uint8 {
    Normal = 0,
    Wireframe,
    Points
};

struct renderer_api {
    //struct platform_state* plat_state; // platform-specific state
    uint64 frame_number;

    virtual bool32 initialize(const char* application_name, struct platform_state* plat_state) = 0;
    virtual void shutdown() = 0;
    virtual void resized(uint16 width, uint16 height) = 0;

    virtual bool32 begin_frame(real32 delta_time) = 0;
    virtual bool32 end_frame(real32 delta_time) = 0;

    virtual bool32 set_draw_mode(render_draw_mode mode) = 0;
    virtual bool32 disable_depth_test() = 0;
    virtual bool32 enable_depth_test() = 0;

    virtual bool32 set_highlight_mode(bool32 enabled) = 0;

    virtual void create_texture(struct texture_2D* texture, const uint8* data) = 0;
    virtual void destroy_texture(struct texture_2D* texture) = 0;
    virtual void create_mesh(triangle_geometry* mesh, 
                             uint32 num_verts, const void* vertices,
                             uint32 num_inds, const uint32* indices,
                             const ShaderDataType* attributes) = 0;
    virtual void destroy_mesh(triangle_geometry* mesh) = 0;
    virtual bool32 create_shader(shader* shader_prog, const uint8* shader_source, uint64 num_bytes) = 0;
    virtual void destroy_shader(shader* shader_prog) = 0;
    virtual bool32 create_framebuffer(frame_buffer* fbo, 
                                      int num_attachments, const frame_buffer_attachment* attachments) = 0;
    virtual void destroy_framebuffer(frame_buffer* fbo) = 0;

    virtual void use_shader(shader* shader_prog) = 0;
    virtual void draw_geometry(triangle_geometry* geom) = 0;
    virtual void draw_geometry(triangle_geometry* geom, uint32 start_idx, uint32 num_inds) = 0;
    virtual void draw_geometry_lines(triangle_geometry* geom) = 0;
    virtual void draw_geometry_points(triangle_geometry* geom) = 0;

    virtual void set_viewport(uint32 x, uint32 y, uint32 width, uint32 height) = 0;
    virtual void clear_viewport(real32 r, real32 g, real32 b, real32 a) = 0;

    // uniforms
    virtual void upload_uniform_float( shader* shader_prog, const char* uniform_name, float  value) = 0;
    virtual void upload_uniform_float2(shader* shader_prog, const char* uniform_name, float* values) = 0;
    virtual void upload_uniform_float3(shader* shader_prog, const char* uniform_name, float* values) = 0;
    virtual void upload_uniform_float4(shader* shader_prog, const char* uniform_name, float* values) = 0;
    virtual void upload_uniform_float4x4(shader* shader_prog, const char* uniform_name, float* values) = 0;
    virtual void upload_uniform_int( shader* shader_prog, const char* uniform_name, int  value) = 0;
    virtual void upload_uniform_int2(shader* shader_prog, const char* uniform_name, int* values) = 0;
    virtual void upload_uniform_int3(shader* shader_prog, const char* uniform_name, int* values) = 0;
    virtual void upload_uniform_int4(shader* shader_prog, const char* uniform_name, int* values) = 0;
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
    uint32 handle; // handle to the gpu version of this data

    uint16 width;
    uint16 height;
    uint16 num_channels;
    uint16 flag;
};

struct shader {
    uint32 handle; // handle to the gpu version of this data
};

enum class frame_buffer_texture_format {
    None = 0,

    // Color
    RED8,
    //RGB8,
    RGBA8,
    RGBA16F,
    RGBA32F,
    R32F,
    //RGB32F,
    //RG32F,

    // Depth/stencil
    //DEPTH32F,
    DEPTH24STENCIL8,

    // Defaults
    Depth = DEPTH24STENCIL8
};

struct frame_buffer_attachment {
    uint32 handle;

    frame_buffer_texture_format texture_format;
    laml::Vec4 clear_color;
};

struct frame_buffer {
    uint32 handle;

    uint32 width;
    uint32 height;

    uint32 num_attachments;
    frame_buffer_attachment attachments[6];
};

struct render_command {
    laml::Mat4 model_matrix;
    triangle_geometry geom;
    uint32 material_handle;
};

struct render_packet {
    memory_arena* arena;

    real32 delta_time;

    laml::Mat4 projection_matrix;
    laml::Mat4 view_matrix;

    laml::Vec3 camera_pos;
    laml::Quat camera_orientation;

    uint32 num_commands;
    render_command* commands;

    // for collision visualization
    collision_grid* col_grid;
    collision_capsule capsule;
    render_command capsule_geom;

    collision_sector sector;
    uint32 num_tris;
    uint32* triangle_indices;

    uint32 num_intersecting_tris;
    uint32* intersecting_triangle_indices;
};