#pragma once

#include "Render_Types.h"

struct stat_mesh_data;
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

void renderer_create_mesh(struct triangle_mesh* mesh, 
                          uint32 num_verts, const void* vertices,
                          uint32 num_inds, const uint32* indices);
void renderer_destroy_mesh(struct triangle_mesh* mesh);

void renderer_shader_create(struct shader* shader_prog, const char* shader_source);
void renderer_shader_destroy(struct shader* shader_prog);