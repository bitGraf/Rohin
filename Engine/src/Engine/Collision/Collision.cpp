#include "Collision.h"

#include "Engine/Memory/Memory_Arena.h"

void collision_create_grid(memory_arena* arena, collision_grid* grid, laml::Vec3 origin, real32 cell_size, 
                           uint16 num_levels, uint16 num_rows, uint16 num_cols) {

    grid->origin = origin;
    grid->cell_size = cell_size;
    grid->num_levels = num_levels;
    grid->num_cols = num_cols;
    grid->num_rows = num_rows;

    grid->cells = PushArray(arena, collision_grid_cell**, num_levels);
    for (uint16 level = 0; level < num_levels; level++) {
        grid->cells[level] = PushArray(arena, collision_grid_cell*, num_rows);
        for (uint16 row = 0; row < num_rows; row++) {
            grid->cells[level][row] = PushArray(arena, collision_grid_cell, num_cols);
            for (uint16 col = 0; col < num_cols; col++) {
                grid->cells[level][row][col].num_surfaces = 0;
                grid->cells[level][row][col].surfaces = nullptr;

#if 1
                // temporary
                int16 center_level = num_levels / 2;
                int16 center_row = num_rows / 2;
                int16 center_col = num_cols / 2;

                int16 dl = level - center_level;
                int16 dr = row - center_row;
                int16 dc = col - center_col;
                
                real32 dist_sq = (real32)(dl*dl + dr*dr + dc*dc);

                if (dist_sq < 16.0f) {
                    grid->cells[level][row][col].num_surfaces = 1;
                }
#endif
            }
        }
    }
}