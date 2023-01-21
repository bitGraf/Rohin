#pragma once

#include "Engine/Resources/Resource_Types.h"

enum class mesh_file_result : int32 {
    error = -1,
    is_static,
    is_animated
};

bool32 resource_init(memory_arena* arena);
void resource_shutdown();
memory_arena* resource_get_arena();

mesh_file_result resource_load_mesh_file(const char* resource_file_name, 
                                         triangle_geometry** mesh,
                                         skeleton** anim_data,
                                         animation** animations, uint32* num_anims);