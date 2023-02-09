#pragma once

#include "Engine/Resources/Resource_Types.h"

struct collision_grid;

enum class mesh_file_result : int32 {
    error = -1,
    is_static,
    is_animated
};

bool32 resource_init(memory_arena* arena);
void resource_shutdown();
memory_arena* resource_get_arena();

RHAPI mesh_file_result resource_load_mesh_file(const char* resource_file_name,
                                         triangle_geometry* mesh,
                                         skeleton* anim_data,
                                         animation** animations, uint32* num_anims);
RHAPI mesh_file_result resource_load_mesh_file_for_level(const char* resource_file_name,
                                                         triangle_geometry* geom,
                                                         collision_grid* grid);
RHAPI bool32 resource_load_debug_mesh_data(const char* resource_file_name, debug_geometry* geom);

//bool32 resource_load_anim_file(animation* anim);
bool32 resource_load_shader_file(const char* resource_file_name,
                                 shader* shader_prog);

RHAPI bool32 resource_load_texture_file(const char* resource_file_name,
                                        texture_2D* texture);