#pragma once

#include "Engine/Resources/Resource_Types.h"

struct collision_grid;
struct mesh_file;

bool32 resource_init(memory_arena* arena);
void resource_shutdown();
memory_arena* resource_get_arena();

RHAPI bool32 resource_load_level_file(const char* resource_file_name, level_data* data);
RHAPI bool32 resource_load_mesh(const char* resource_file_name, resource_mesh* mesh);

RHAPI bool32 resource_load_debug_mesh_into_geometry(const char* resource_file_name, render_geometry* geom);
RHAPI bool32 resource_load_debug_mesh_data(const char* resource_file_name, debug_geometry* geom);

bool32 resource_load_shader_file(const char* resource_file_name,
                                 shader* shader_prog);

RHAPI bool32 resource_load_texture_file(const char* resource_file_name,
                                        render_texture_2D* texture);

/* (MAYBE) GET RID OF THESE! */

#if 0
RHAPI mesh_file_result resource_load_mesh_file(const char* resource_file_name,
                                         render_geometry* mesh,
                                         skeleton* anim_data,
                                         animation** animations, uint32* num_anims);
RHAPI bool32 resource_load_mesh(mesh_file* file_data, render_geometry* geom_out);
RHAPI bool32 resource_load_mesh_into_grid(mesh_file* file_data, collision_grid* grid, const laml::Mat4& transform);

// OLD - delete when done
RHAPI bool32 resource_load_mesh_file_for_level(const char* resource_file_name,
                                                         render_geometry* geom,
                                                         collision_grid* grid,
                                                         laml::Mat4 transform,
                                                         bool32 reserve);

#endif