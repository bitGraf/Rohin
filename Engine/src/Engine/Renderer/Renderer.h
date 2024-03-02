#pragma once

#include "Render_Types.h"

global_variable ShaderDataType static_mesh_attribute_list[6] = {
    ShaderDataType::Float3,
    ShaderDataType::Float3,
    ShaderDataType::Float3,
    ShaderDataType::Float3,
    ShaderDataType::Float2,
    ShaderDataType::None // null-terminator
};
global_variable ShaderDataType dynamic_mesh_attribute_list[8] = {
    ShaderDataType::Float3,
    ShaderDataType::Float3,
    ShaderDataType::Float3,
    ShaderDataType::Float3,
    ShaderDataType::Float2,
    ShaderDataType::Int4,
    ShaderDataType::Float4,
    ShaderDataType::None // null-terminator
};

//struct stat_mesh_data;
struct platform_state;
struct memory_arena;
struct event_context;

bool32 renderer_initialize(memory_arena* arena, const char* application_name, platform_state* plat_state);
bool32 renderer_create_pipeline();
void renderer_shutdown();

void renderer_resized(uint32 width, uint32 height);
void renderer_on_event(uint16 code, void* sender, void* listener, event_context context);

bool32 renderer_draw_frame(render_packet* packet);

bool32 renderer_begin_wireframe();
bool32 renderer_end_wireframe();

// functions to create/destroy render objects
// things like meshes, shaders, textures, etc.

void renderer_create_texture(struct render_texture_2D* texture, const void* data, bool32 is_hdr);
void renderer_destroy_texture(struct render_texture_2D* texture);

RHAPI void renderer_create_mesh(render_geometry* mesh, 
                          uint32 num_verts, const void* vertices,
                          uint32 num_inds, const uint32* indices,
                          const ShaderDataType* attributes);
void renderer_destroy_mesh(render_geometry* mesh);

bool32 renderer_create_shader(shader* shader_prog, const uint8* shader_source, uint64 num_bytes);
void renderer_destroy_shader(shader* shader_prog);
bool32 renderer_create_framebuffer(frame_buffer* fbo, 
                                   int num_attachments, 
                                   const frame_buffer_attachment* attachments);
bool32 renderer_create_framebuffer_cube(frame_buffer* fbo, 
                                        int num_attachments, 
                                        const frame_buffer_attachment* attachments,
                                        bool32 gen_mipmaps);
void renderer_destroy_framebuffer(frame_buffer* fbo);

void renderer_use_shader(shader* shader_prog);
void renderer_draw_geometry(render_geometry* geom);
void renderer_draw_geometry(render_geometry* geom, uint32 start_idx, uint32 num_inds);
void renderer_draw_geometry(render_geometry* geom, render_material* mat);
void renderer_draw_geometry_lines(render_geometry* geom);
void renderer_draw_geometry_points(render_geometry* geom);