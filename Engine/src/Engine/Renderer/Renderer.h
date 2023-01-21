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

bool32 renderer_initialize(memory_arena* arena, const char* application_name, platform_state* plat_state);
void renderer_shutdown();

void renderer_resized(uint16 width, uint16 height);

bool32 renderer_draw_frame(render_packet* packet);

// functions to create/destroy render objects
// things like meshes, shaders, textures, etc.

void renderer_create_texture(struct texture_2D* texture, const uint8* data);
void renderer_destroy_texture(struct texture_2D* texture);

void renderer_create_mesh(triangle_geometry* mesh, 
                          uint32 num_verts, const void* vertices,
                          uint32 num_inds, const uint32* indices,
                          const ShaderDataType* attributes);
void renderer_destroy_mesh(triangle_geometry* mesh);

void renderer_create_shader(struct shader* shader_prog, const char* shader_source);
void renderer_destroy_shader(struct shader* shader_prog);