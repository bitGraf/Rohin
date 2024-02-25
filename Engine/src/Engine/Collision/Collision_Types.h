#pragma once

#include "Engine/Defines.h"
#include <laml/laml.hpp>
//#include "Engine/Renderer/Render_Types.h"

struct render_geometry;

struct sweep_result {
    laml::Vec3 position;
    laml::Vec3 normal;
    real32 distance;
    uint32 face_index;
};

struct collision_triangle {
    laml::Vec3 v1;
    laml::Vec3 v2;
    laml::Vec3 v3;
};

struct collision_capsule {
    laml::Vec3 A;
    laml::Vec3 B;
    real32 radius;
};

struct collision_sphere {
    laml::Vec3 C;
    real32 radius;
};

struct collision_sector {
    bool32 inside;

    int32 x_min;
    int32 x_max;
    int32 y_min;
    int32 y_max;
    int32 z_min; 
    int32 z_max;
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

    render_geometry* geom;
};