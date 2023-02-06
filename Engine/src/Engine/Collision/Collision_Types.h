#pragma once

//#include "Engine/Defines.h"
//#include <laml/laml.hpp>
#include "Engine/Renderer/Render_Types.h"

struct collision_triangle {
    laml::Vec3 v1;
    laml::Vec3 v2;
    laml::Vec3 v3;
};

struct collision_grid_cell {
    uint32 num_surfaces;
    uint32 _num_to_allocate;

    uint32* surfaces;
};

struct collision_grid {
    laml::Vec3 origin;
    real32 cell_size;

    laml::Vec3 min_point;
    laml::Vec3 max_point;

    uint16 num_x;
    uint16 num_y;
    uint16 num_z;
    uint16 _num_tris_to_allocate;

    uint32 num_triangles;
    uint32 num_filled_cells;

    collision_triangle* triangles;
    collision_grid_cell*** cells; // cells[level][row][col]

    triangle_geometry geom;
};