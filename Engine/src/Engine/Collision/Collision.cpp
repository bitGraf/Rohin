#include "Collision.h"

#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Renderer/Renderer.h"

void collision_create_grid(memory_arena* arena, collision_grid* grid, laml::Vec3 origin, real32 cell_size, 
                           uint16 num_x, uint16 num_y, uint16 num_z) {

    grid->origin = origin;
    grid->cell_size = cell_size;
    grid->num_x = num_x;
    grid->num_y = num_y;
    grid->num_z = num_z;

    uint32 num_filled_cells = 0;

    grid->cells = PushArray(arena, collision_grid_cell**, num_x);
    for (uint16 x = 0; x < num_x; x++) {
        grid->cells[x] = PushArray(arena, collision_grid_cell*, num_y);
        for (uint16 y = 0; y < num_y; y++) {
            grid->cells[x][y] = PushArray(arena, collision_grid_cell, num_z);
            for (uint16 z = 0; z < num_z; z++) {
                grid->cells[x][y][z].num_surfaces = 0;
                grid->cells[x][y][z].surfaces = nullptr;

#if 1
                // temporary
                int16 center_x = num_x / 2;
                int16 center_y = num_y / 2;
                int16 center_z = num_z / 2;

                int16 dx = x - center_x;
                int16 dy = y - center_y;
                int16 dz = z - center_z;
                
                real32 dist_sq = (real32)(dx*dx + dy*dy + dz*dz);

                if (dist_sq < 16.0f) {
                    grid->cells[x][y][z].num_surfaces = 2;
                    num_filled_cells++;
                }
#endif
            }
        }
    }

    // create mesh geometry for rendering
    typedef laml::Vec3 col_grid_vert;
    real32 s = 1.0f;
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

    uint32 num_inds = num_filled_cells * num_inds_per_cube;
    uint32 num_verts = num_filled_cells * num_verts_per_cube;

    memory_index arena_save = arena->Used;
    col_grid_vert* verts = PushArray(arena, col_grid_vert, num_verts);
    uint32* inds = PushArray(arena, uint32, num_inds);

    uint32 current_vert = 0;
    uint32 current_ind = 0;
    for (uint16 x = 0; x < num_x; x++) {
        real32 cube_x = (real32)x - (real32)num_x / 2.0f;
        for (uint16 y = 0; y < num_y; y++) {
            real32 cube_y = (real32)y - (real32)num_y / 2.0f;
            for (uint16 z = 0; z < num_z; z++) {
                real32 cube_z = (real32)z - (real32)num_z / 2.0f;
                if (grid->cells[x][y][z].num_surfaces > 0) {
                    for (int n = 0; n < num_inds_per_cube; n++) {
                        inds[current_ind++] = current_vert + cube_inds[n];
                    }

                    laml::Vec3 cube_origin = origin + laml::Vec3(cube_x, cube_y, cube_z);
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