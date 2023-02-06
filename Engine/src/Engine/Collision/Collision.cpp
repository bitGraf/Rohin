#include "Collision.h"

#include "Engine/Core/Logger.h"
#include "Engine/Memory/MemoryUtils.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Renderer/Renderer.h"

void collision_create_grid(memory_arena* arena, collision_grid* grid, laml::Vec3 origin, real32 cell_size, 
                           uint16 num_x, uint16 num_y, uint16 num_z) {

    memory_zero(grid, sizeof(collision_grid));
    grid->origin = origin;
    grid->cell_size = cell_size;
    grid->num_x = num_x;
    grid->num_y = num_y;
    grid->num_z = num_z;

    grid->cells = PushArray(arena, collision_grid_cell**, num_x);
    for (uint16 x = 0; x < num_x; x++) {
        grid->cells[x] = PushArray(arena, collision_grid_cell*, num_y);
        for (uint16 y = 0; y < num_y; y++) {
            grid->cells[x][y] = PushArray(arena, collision_grid_cell, num_z);
            for (uint16 z = 0; z < num_z; z++) {
                grid->cells[x][y][z].num_surfaces = 0;
                grid->cells[x][y][z]._num_to_allocate = 0;
                grid->cells[x][y][z].surfaces = nullptr;
            }
        }
    }

    
}

void collision_grid_add_triangle(memory_arena* arena, collision_grid* grid, collision_triangle triangle, bool32 reserve) {
    real32 max_x_world = laml::max(laml::Vec3(triangle.v1.x, triangle.v2.x, triangle.v3.x));
    real32 max_y_world = laml::max(laml::Vec3(triangle.v1.y, triangle.v2.y, triangle.v3.y));
    real32 max_z_world = laml::max(laml::Vec3(triangle.v1.z, triangle.v2.z, triangle.v3.z));

    real32 min_x_world = laml::min(laml::Vec3(triangle.v1.x, triangle.v2.x, triangle.v3.x));
    real32 min_y_world = laml::min(laml::Vec3(triangle.v1.y, triangle.v2.y, triangle.v3.y));
    real32 min_z_world = laml::min(laml::Vec3(triangle.v1.z, triangle.v2.z, triangle.v3.z));

    int32 max_x = (grid->num_x/2) + (int32)(floor(max_x_world - grid->origin.x) / grid->cell_size);
    int32 max_y = (grid->num_y/2) + (int32)(floor(max_y_world - grid->origin.y) / grid->cell_size);
    int32 max_z = (grid->num_z/2) + (int32)(floor(max_z_world - grid->origin.z) / grid->cell_size);

    int32 min_x = (grid->num_x/2) + (int32)(floor(min_x_world - grid->origin.x) / grid->cell_size);
    int32 min_y = (grid->num_y/2) + (int32)(floor(min_y_world - grid->origin.y) / grid->cell_size);
    int32 min_z = (grid->num_z/2) + (int32)(floor(min_z_world - grid->origin.z) / grid->cell_size);

    // TODO: check if these bounds fit within the whole grid!!

    uint32 triangle_idx = 0;
    if (reserve) {
        grid->_num_tris_to_allocate++;
    } else {
        if (grid->num_triangles == 0) {
            grid->triangles = PushArray(arena, collision_triangle, grid->_num_tris_to_allocate);
        }
        triangle_idx = grid->num_triangles;
        grid->triangles[grid->num_triangles++] = triangle;
    }

    for (int x = min_x; x <= max_x; x++) {
        real32 cube_x = (real32)x - (real32)grid->num_x / 2.0f;
        for (int y = min_y; y <= max_y; y++) {
            real32 cube_y = (real32)y - (real32)grid->num_y / 2.0f;
            for (int z = min_z; z <= max_z; z++) {
                real32 cube_z = (real32)z - (real32)grid->num_z / 2.0f;
                laml::Vec3 cube_center = grid->origin + laml::Vec3(cube_x, cube_y, cube_z)*grid->cell_size;
                laml::Vec3 o(0.5f, 0.5f, 0.5f);
                cube_center = cube_center + o;

                //RH_TRACE("cube_center: [%.1f,%.1f,%.1f]", cube_center.x, cube_center.y, cube_center.z);

                // put triangle into 'unit-cube' space to test
                collision_triangle tri_mod;
                tri_mod.v1 = (triangle.v1 / grid->cell_size) - cube_center;
                tri_mod.v2 = (triangle.v2 / grid->cell_size) - cube_center;
                tri_mod.v3 = (triangle.v3 / grid->cell_size) - cube_center;

                if (triangle_cube_intersect(tri_mod) == INSIDE) {
                    collision_grid_cell* cell = &grid->cells[x][y][z];
                    // add this triangle to this cell
                    if (reserve) {
                        if (cell->_num_to_allocate == 0) {
                            grid->num_filled_cells++; // only increment this on the first time
                        }
                        // just increment the count
                        cell->_num_to_allocate++;
                    } else {
                        // actually store the data
                        // just increment the count
                        if (cell->num_surfaces == 0) {
                            cell->surfaces = PushArray(arena, uint32, cell->_num_to_allocate);
                        }
                        cell->surfaces[cell->num_surfaces++] = triangle_idx;
                    }
                }
            }
        }
    }
}

void collision_grid_finalize(memory_arena* arena, collision_grid* grid) {
    RH_INFO("Finalizing collision grid!\n          %d cells filled in!", grid->num_filled_cells);
    // create mesh geometry for rendering
    typedef laml::Vec3 col_grid_vert;
    real32 s = grid->cell_size;
    col_grid_vert cube_verts[] = {
                                 {0, 0, 0},// 0
                                 {s, 0, 0},// 1
                                 {s, s, 0},// 2
                                 {0, s, 0},// 3

                                 {0, 0, s}, // 4
                                 {s, 0, s}, // 5
                                 {s, s, s}, // 6
                                 {0, s, s}, // 7
    };
    uint32 cube_inds[] = {
                         // bottom face
                         0, 1,
                         1, 2,
                         2, 3,
                         3, 0,

                         // top face
                         4, 5,
                         5, 6,
                         6, 7,
                         7, 4,

                         //verticals
                         0, 4,
                         1, 5,
                         2, 6,
                         3, 7
    };
    const uint32 num_inds_per_cube = 24;
    const uint32 num_verts_per_cube = 8;
    const ShaderDataType cube_attrs[] = {ShaderDataType::Float3, ShaderDataType::None};

    uint32 num_inds  = grid->num_filled_cells * num_inds_per_cube;
    uint32 num_verts = grid->num_filled_cells * num_verts_per_cube;

    memory_index arena_save = arena->Used;
    col_grid_vert* verts = PushArray(arena, col_grid_vert, num_verts);
    uint32* inds = PushArray(arena, uint32, num_inds);

    uint32 current_vert = 0;
    uint32 current_ind = 0;
    for (uint16 x = 0; x < grid->num_x; x++) {
        real32 cube_x = (real32)x - (real32)grid->num_x / 2.0f;
        for (uint16 y = 0; y < grid->num_y; y++) {
            real32 cube_y = (real32)y - (real32)grid->num_y / 2.0f;
            for (uint16 z = 0; z < grid->num_z; z++) {
                real32 cube_z = (real32)z - (real32)grid->num_z / 2.0f;
                if (grid->cells[x][y][z].num_surfaces > 0) {
                    for (int n = 0; n < num_inds_per_cube; n++) {
                        inds[current_ind++] = current_vert + cube_inds[n];
                    }

                    laml::Vec3 cube_origin = grid->origin + laml::Vec3(cube_x, cube_y, cube_z)*grid->cell_size;
                    for (int n = 0; n < num_verts_per_cube; n++) {
                        verts[current_vert++] = cube_origin + cube_verts[n];
                    }
                }
            }
        }
    }

    renderer_create_mesh(&grid->geom, num_verts, verts, num_inds, inds, cube_attrs);

    arena->Used = arena_save;
}