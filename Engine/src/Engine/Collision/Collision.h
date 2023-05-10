#pragma once

#include "Collision_Types.h"

struct memory_arena;

RHAPI void collision_create_grid(memory_arena* arena, collision_grid* grid, laml::Vec3 origin, real32 cell_size, 
                                 uint16 num_x, uint16 num_y, uint16 num_z);
RHAPI void collision_grid_add_triangle(memory_arena* arena, collision_grid* grid, collision_triangle triangle, bool32 reserve);
RHAPI void collision_grid_finalize(memory_arena* arena, collision_grid* grid);

RHAPI void collision_create_capsule(collision_capsule* collider, triangle_geometry* geom, real32 height, real32 radius, laml::Vec3 N);
RHAPI void collision_create_sphere(collision_sphere* collider, triangle_geometry* geom, real32 radius);

RHAPI void collision_grid_get_sector_capsule(collision_grid* grid, collision_sector* sector, laml::Vec3 P0, laml::Vec3 P1, real32 radius);
RHAPI void collision_grid_get_sector_sphere( collision_grid* grid, collision_sector* sector, laml::Vec3 C, real32 radius);

RHAPI uint32* collision_get_unique_triangles(collision_grid* grid, collision_sector* sector, memory_arena* arena, uint32* num_tris);

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

RHAPI bool32 triangle_capsule_intersect(collision_triangle triangle, collision_capsule capsule, laml::Vec3& contact_point);
RHAPI bool32 triangle_sphere_intersect(collision_triangle triangle, collision_sphere sphere, laml::Vec3& contact_point);

RHAPI bool32 sweep_sphere_triangles(uint32 num_tris, collision_triangle* triangles, laml::Vec3 sphere_center, real32 sphere_radius,
                              laml::Vec3 unit_direction, real32 distance,
                              sweep_result& hit, laml::Vec3& tri_normal_out);


RHAPI bool32 segment_intersect_triangle(laml::Vec3 p, laml::Vec3 q, laml::Vec3 a, laml::Vec3 b, laml::Vec3 c,
                                        real32& u, real32& v, real32& w, real32& t);