#pragma once

#include "Collision_Types.h"

struct memory_arena;

RHAPI void collision_create_grid(memory_arena* arena, collision_grid* grid, laml::Vec3 origin, real32 cell_size, 
                                 uint16 num_x, uint16 num_y, uint16 num_z);
RHAPI void collision_grid_add_triangle(memory_arena* arena, collision_grid* grid, collision_triangle triangle, bool32 reserve);
RHAPI void collision_grid_finalize(memory_arena* arena, collision_grid* grid);

#define USING_GPU_GEMS_3_INTERSECTION 0
#if USING_GPU_GEMS_3_INTERSECTION
#define INSIDE 0
#define OUTSIDE 1
RHAPI bool32 triangle_cube_intersect(collision_triangle t);
#else
RHAPI bool32 triangle_cube_intersect(collision_triangle t, laml::Vec3 box_center, laml::Vec3 box_halfsize);
#endif