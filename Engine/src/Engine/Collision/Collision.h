#pragma once

#include "Collision_Types.h"

struct memory_arena;

RHAPI void collision_create_grid(memory_arena* arena, collision_grid* grid, laml::Vec3 origin, real32 cell_size, 
                                 uint16 num_levels, uint16 num_rows, uint16 num_cols);