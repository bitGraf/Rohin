#pragma once

#include "Collision_Types.h"

struct memory_arena;

RHAPI void collision_create_grid(memory_arena* arena, collision_grid* grid, laml::Vec3 origin, real32 cell_size, 
                                 uint16 num_x, uint16 num_y, uint16 num_z);
RHAPI void collision_grid_add_triangle(memory_arena* arena, collision_grid* grid, collision_triangle triangle, bool32 reserve);
RHAPI void collision_grid_finalize(memory_arena* arena, collision_grid* grid);

RHAPI void collision_create_capsule(collision_capsule* capsule, triangle_geometry* geom, real32 height, real32 radius, laml::Vec3 N);

RHAPI void collision_grid_get_sector(collision_grid* grid, collision_sector* sector, collision_capsule* capsule, laml::Vec3 position);

RHAPI laml::Vec3 collision_cell_to_world(collision_grid* grid, uint32 grid_x, uint32 grid_y, uint32 grid_z);
//RHAPI laml::Vec3 collision_world_to_cell(collision_grid* grid, uint32 grid_x, uint32 grid_y, uint32 grid_z);

#define USING_GPU_GEMS_3_INTERSECTION 0
#if USING_GPU_GEMS_3_INTERSECTION
#define INSIDE 0
#define OUTSIDE 1
RHAPI bool32 triangle_cube_intersect(collision_triangle t);
#else
RHAPI bool32 triangle_cube_intersect(collision_triangle t, laml::Vec3 box_center, laml::Vec3 box_halfsize);
#endif

RHAPI bool32 triangle_capsule_intersect(collision_triangle triangle, collision_capsule capsule, laml::Vec3 capsule_position);


RHAPI bool32 segment_intersect_triangle(laml::Vec3 p, laml::Vec3 q, laml::Vec3 a, laml::Vec3 b, laml::Vec3 c,
                                        real32& u, real32& v, real32& w, real32& t);