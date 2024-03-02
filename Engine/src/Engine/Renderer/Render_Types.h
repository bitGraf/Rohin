#pragma once

#include "Engine/Defines.h"
#include <laml/laml.hpp>

#include "Engine/Collision/Collision_Types.h"

struct memory_arena;
struct render_geometry;
struct render_material;
struct shader;
struct ShaderUniform;
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

    virtual bool32 ImGui_Init() = 0;
    virtual bool32 ImGui_begin_frame() = 0;
    virtual bool32 ImGui_end_frame() = 0;
    virtual bool32 ImGui_Shutdown() = 0;

    virtual bool32 set_draw_mode(render_draw_mode mode) = 0;
    virtual bool32 disable_depth_test() = 0;
    virtual bool32 enable_depth_test() = 0;

    virtual bool32 set_highlight_mode(bool32 enabled) = 0;

    virtual void create_texture(struct render_texture_2D* texture, const void* data, bool32 is_hdr) = 0;
    virtual void destroy_texture(struct render_texture_2D* texture) = 0;

    virtual void create_mesh(render_geometry* mesh, 
                             uint32 num_verts, const void* vertices,
                             uint32 num_inds, const uint32* indices,
                             const ShaderDataType* attributes) = 0;
    virtual void destroy_mesh(render_geometry* mesh) = 0;

    virtual bool32 create_shader(shader* shader_prog, const uint8* shader_source, uint64 num_bytes) = 0;
    virtual void destroy_shader(shader* shader_prog) = 0;

    virtual bool32 create_framebuffer(frame_buffer* fbo, 
                                      int num_attachments, 
                                      const frame_buffer_attachment* attachments) = 0;
    virtual bool32 create_framebuffer_cube(frame_buffer* fbo, 
                                           int num_attachments, 
                                           const frame_buffer_attachment* attachments,
                                           bool32 generate_mipmaps) = 0;  
    virtual void destroy_framebuffer(frame_buffer* fbo) = 0;

    virtual void use_shader(shader* shader_prog) = 0;
    virtual void use_framebuffer(frame_buffer *fbuffer) = 0;
    virtual void set_framebuffer_cube_face(frame_buffer* fbuffer, uint32 attach_idx, uint32 slot, uint32 mip_level) = 0;
    virtual void resize_framebuffer_renderbuffer(frame_buffer* fbuffer, uint32 new_width, uint32 new_height) = 0;

    virtual void draw_geometry(render_geometry* geom) = 0;
    virtual void draw_geometry(render_geometry* geom, uint32 start_idx, uint32 num_inds) = 0;
    virtual void draw_geometry(render_geometry* geom, render_material* mat) = 0;
    virtual void draw_geometry_lines(render_geometry* geom) = 0;
    virtual void draw_geometry_points(render_geometry* geom) = 0;

    virtual void bind_texture(uint32 tex_handle, uint32 slot) = 0;
    virtual void bind_texture_cube(uint32 tex_handle, uint32 slot) = 0;

    virtual void set_viewport(uint32 x, uint32 y, uint32 width, uint32 height) = 0;
    virtual void clear_viewport(real32 r, real32 g, real32 b, real32 a) = 0;
    virtual void clear_framebuffer_attachment(frame_buffer_attachment* attach, real32 r, real32 g, real32 b, real32 a) = 0;

    // uniforms
    //virtual void upload_uniform_float(   shader* shader_prog, const char* uniform_name, float  value) = 0;
    //virtual void upload_uniform_float2(  shader* shader_prog, const char* uniform_name, float* values) = 0;
    //virtual void upload_uniform_float3(  shader* shader_prog, const char* uniform_name, float* values) = 0;
    //virtual void upload_uniform_float4(  shader* shader_prog, const char* uniform_name, float* values) = 0;
    //virtual void upload_uniform_float4x4(shader* shader_prog, const char* uniform_name, float* values) = 0;
    //virtual void upload_uniform_int(     shader* shader_prog, const char* uniform_name, int  value) = 0;
    //virtual void upload_uniform_int2(    shader* shader_prog, const char* uniform_name, int* values) = 0;
    //virtual void upload_uniform_int3(    shader* shader_prog, const char* uniform_name, int* values) = 0;
    //virtual void upload_uniform_int4(    shader* shader_prog, const char* uniform_name, int* values) = 0;

    virtual void upload_uniform_float(   ShaderUniform uniform, float  value) = 0;
    virtual void upload_uniform_float2(  ShaderUniform uniform, float* values) = 0;
    virtual void upload_uniform_float3(  ShaderUniform uniform, float* values) = 0;
    virtual void upload_uniform_float4(  ShaderUniform uniform, float* values) = 0;
    virtual void upload_uniform_float4x4(ShaderUniform uniform, float* values) = 0;
    virtual void upload_uniform_int(     ShaderUniform uniform, int  value) = 0;
    virtual void upload_uniform_int2(    ShaderUniform uniform, int* values) = 0;
    virtual void upload_uniform_int3(    ShaderUniform uniform, int* values) = 0;
    virtual void upload_uniform_int4(    ShaderUniform uniform, int* values) = 0;
};

// the actual geometry that the gpu holds onto
// separately, a 'resource' will exist to 
// represent an actual mesh.
struct render_geometry {
    uint32 handle; // handle to the gpu version of this data

    uint32 num_verts;
    uint32 num_inds;
    uint32 flag;
};

struct render_texture_2D {
    uint32 handle; // handle to the gpu version of this data

    uint16 width;
    uint16 height;
    uint16 num_channels;
    uint16 flag;
};

struct render_material {
    laml::Vec3 DiffuseFactor;
    real32 NormalScale;
    real32 AmbientStrength;
    real32 MetallicFactor;
    real32 RoughnessFactor;
    laml::Vec3 EmissiveFactor;
    uint32 flag;

    render_texture_2D DiffuseTexture;
    render_texture_2D NormalTexture;
    render_texture_2D AMRTexture;
    render_texture_2D EmissiveTexture;
};

struct shader {
    uint32 handle; // handle to the gpu version of this data
};

struct ShaderUniform {
    uint32 Location;
    uint32 SamplerID; // only for samplers
};

typedef ShaderUniform ShaderUniform_int;

typedef ShaderUniform ShaderUniform_float;
typedef ShaderUniform ShaderUniform_vec2;
typedef ShaderUniform ShaderUniform_vec3;
typedef ShaderUniform ShaderUniform_vec4;
typedef ShaderUniform ShaderUniform_mat3;
typedef ShaderUniform ShaderUniform_mat4;
typedef ShaderUniform ShaderUniform_sampler2D;
typedef ShaderUniform ShaderUniform_samplerCube;

struct ShaderUniform_Light {
    uint32 Location;

    ShaderUniform_vec3 Position;
    ShaderUniform_vec3 Direction;
    ShaderUniform_vec3 Color;
    ShaderUniform_float Strength;
    ShaderUniform_float Inner;
    ShaderUniform_float Outer;
};

enum class frame_buffer_texture_format {
    None = 0,

    // Color
    RED8,
    //RGB8,
    RGBA8,
    RGBA16F,
    RG16F,
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
    render_geometry geom;
    render_material material;
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
    uint32 draw_colliders;
    collision_grid* col_grid;
    render_command collider_geom;

    collision_sector sector;
    uint32 num_tris;
    uint32* triangle_indices;

    uint32 num_intersecting_tris;
    uint32* intersecting_triangle_indices;

    laml::Vec3 contact_point;
};