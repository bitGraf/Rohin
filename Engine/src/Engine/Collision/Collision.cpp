#include "Collision.h"

#include "Engine/Core/Logger.h"
#include "Engine/Memory/MemoryUtils.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Renderer/Renderer.h"

#include "Engine/Resources/Resource_Manager.h"

void collision_create_grid(memory_arena* arena, collision_grid* grid, laml::Vec3 origin, real32 cell_size, 
                           uint16 num_x, uint16 num_y, uint16 num_z) {

    RH_INFO("Creating collision grid...");
    memory_zero(grid, sizeof(collision_grid));
    grid->origin = origin;
    grid->cell_size = cell_size;
    grid->num_x = num_x;
    grid->num_y = num_y;
    grid->num_z = num_z;

    grid->geom = PushStruct(arena, triangle_geometry);

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

    int32 max_x = (grid->num_x/2) + (int32)(floor((max_x_world - grid->origin.x) / grid->cell_size));
    int32 max_y = (grid->num_y/2) + (int32)(floor((max_y_world - grid->origin.y) / grid->cell_size));
    int32 max_z = (grid->num_z/2) + (int32)(floor((max_z_world - grid->origin.z) / grid->cell_size));
    int32 min_x = (grid->num_x/2) + (int32)(floor((min_x_world - grid->origin.x) / grid->cell_size));
    int32 min_y = (grid->num_y/2) + (int32)(floor((min_y_world - grid->origin.y) / grid->cell_size));
    int32 min_z = (grid->num_z/2) + (int32)(floor((min_z_world - grid->origin.z) / grid->cell_size));

    max_x = max_x >= grid->num_x ? grid->num_x-1 : max_x;
    max_y = max_y >= grid->num_y ? grid->num_y-1 : max_y;
    max_z = max_z >= grid->num_z ? grid->num_z-1 : max_z;
    min_x = min_x < 0 ? 0 : min_x;
    min_y = min_y < 0 ? 0 : min_y;
    min_z = min_z < 0 ? 0 : min_z;

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
        real32 cube_x = (real32)(x - (grid->num_x / 2));
        for (int y = min_y; y <= max_y; y++) {
            real32 cube_y = (real32)(y - (grid->num_y / 2));
            for (int z = min_z; z <= max_z; z++) {
                real32 cube_z = (real32)(z - (grid->num_z / 2));
                laml::Vec3 cube_center = laml::Vec3(cube_x, cube_y, cube_z) + laml::Vec3(0.5f, 0.5f, 0.5f);
                cube_center = (cube_center* grid->cell_size + grid->origin);

                //RH_TRACE("cube_center: [%.3f,%.3f,%.3f]", cube_center.x, cube_center.y, cube_center.z);

#if USING_GPU_GEMS_3_INTERSECTION
                // put triangle into 'unit-cube' space to test
                collision_triangle tri_mod;
                tri_mod.v1 = (triangle.v1 - cube_center) / grid->cell_size;
                tri_mod.v2 = (triangle.v2 - cube_center) / grid->cell_size;
                tri_mod.v3 = (triangle.v3 - cube_center) / grid->cell_size;

                if (triangle_cube_intersect(tri_mod) == INSIDE) {
#else
                if (triangle_cube_intersect(triangle, cube_center, laml::Vec3(grid->cell_size/2.0f))) {
#endif
                    collision_grid_cell* cell = &grid->cells[x][y][z];
                    //RH_INFO("Intersects! [%d,%d,%d] => %d", x, y, z, triangle_idx);
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
                    for (uint32 n = 0; n < num_inds_per_cube; n++) {
                        inds[current_ind++] = current_vert + cube_inds[n];
                    }

                    laml::Vec3 cube_origin = grid->origin + laml::Vec3(cube_x, cube_y, cube_z)*grid->cell_size;
                    for (uint32 n = 0; n < num_verts_per_cube; n++) {
                        verts[current_vert++] = cube_origin + cube_verts[n];
                    }
                }
            }
        }
    }

    renderer_create_mesh(grid->geom, num_verts, verts, num_inds, inds, cube_attrs);

    arena->Used = arena_save;
}

laml::Vec3 collision_cell_to_world(collision_grid* grid, uint32 grid_x, uint32 grid_y, uint32 grid_z) {
    real32 cube_x = (real32)grid_x - (real32)grid->num_x / 2.0f;
    real32 cube_y = (real32)grid_y - (real32)grid->num_y / 2.0f;
    real32 cube_z = (real32)grid_z - (real32)grid->num_z / 2.0f;

    return grid->origin + laml::Vec3(cube_x, cube_y, cube_z)*grid->cell_size;
}


void collision_create_capsule(collision_capsule* collider, triangle_geometry* geom, real32 height, real32 radius, laml::Vec3 N) {
    collider->radius = radius;
    real32 s = height - (2*radius); // distance between A and B

    N = laml::normalize(N);
    collider->A = radius*N;
    collider->B = (s + radius)*N;

    if (geom) {
        // generate capsule geometry for debug rendering
        debug_geometry sphere_bot;
        resource_load_debug_mesh_data("Data/Models/debug/sphere_bot.stl", &sphere_bot);
        debug_geometry sphere_top;
        resource_load_debug_mesh_data("Data/Models/debug/sphere_top.stl", &sphere_top);
        debug_geometry open_cylinder;
        resource_load_debug_mesh_data("Data/Models/debug/open_cylinder.stl", &open_cylinder);

        debug_geometry combined;
        combined.num_verts = sphere_bot.num_verts + sphere_top.num_verts + open_cylinder.num_verts;
        combined.num_inds  = sphere_bot.num_inds  + sphere_top.num_inds  + open_cylinder.num_inds;
        
        memory_arena* arena = resource_get_arena();
        combined.vertices = PushArray(arena, debug_geometry::debug_vertex, combined.num_verts);
        combined.indices  = PushArray(arena, uint32, combined.num_inds);

        uint32 curr_vert = 0;
        uint32 curr_idx = 0;
        uint32 idx_offset = 0;
        for (uint32 n = 0; n < sphere_bot.num_verts; n++) {
            // bottom verts need to be scaled by radius, then have capsule.A added
            combined.vertices[curr_vert].position = (sphere_bot.vertices[n].position * collider->radius) + collider->A;
            combined.vertices[curr_vert++].normal = sphere_bot.vertices[n].normal;

            combined.indices[curr_idx++] = idx_offset + sphere_bot.indices[n];
        }
        idx_offset = curr_idx;
        for (uint32 n = 0; n < sphere_top.num_verts; n++) {
            // top verts need to be scaled by radius, then have capsule.B added
            combined.vertices[curr_vert].position = (sphere_top.vertices[n].position * collider->radius) + collider->B;
            combined.vertices[curr_vert++].normal =  sphere_top.vertices[n].normal;

            combined.indices[curr_idx++] = idx_offset + sphere_top.indices[n];
        }
        idx_offset = curr_idx;
        real32 capsule_inner_height = laml::length(collider->B - collider->A);
        laml::Vec3 cylinder_scale(collider->radius, capsule_inner_height, collider->radius);
        for (uint32 n = 0; n < open_cylinder.num_verts; n++) {
            // cylinder verts need to be scaled (by radius in the x/z plane, and the height in the y direction), then have capsule.A added
            combined.vertices[curr_vert].position = (open_cylinder.vertices[n].position * cylinder_scale) + collider->A;
            combined.vertices[curr_vert++].normal =  open_cylinder.vertices[n].normal;

            combined.indices[curr_idx++] = idx_offset + open_cylinder.indices[n];
        }

        // now create geometry on the gpu for rendering
        ShaderDataType debug_attr[] = {ShaderDataType::Float3, ShaderDataType::Float3, ShaderDataType::None};
        renderer_create_mesh(geom, combined.num_verts, combined.vertices, combined.num_inds, combined.indices, debug_attr);
    }
}

void collision_create_sphere(collision_sphere* collider, triangle_geometry* geom, real32 radius) {
    collider->radius = radius;

    collider->C = laml::Vec3(0.0f);

    if (geom) {
        // generate capsule geometry for debug rendering
        debug_geometry sphere_bot;
        resource_load_debug_mesh_data("Data/Models/debug/sphere_bot.stl", &sphere_bot);
        debug_geometry sphere_top;
        resource_load_debug_mesh_data("Data/Models/debug/sphere_top.stl", &sphere_top);

        debug_geometry combined;
        combined.num_verts = sphere_bot.num_verts + sphere_top.num_verts;
        combined.num_inds  = sphere_bot.num_inds  + sphere_top.num_inds;
        
        memory_arena* arena = resource_get_arena();
        combined.vertices = PushArray(arena, debug_geometry::debug_vertex, combined.num_verts);
        combined.indices  = PushArray(arena, uint32, combined.num_inds);

        uint32 curr_vert = 0;
        uint32 curr_idx = 0;
        uint32 idx_offset = 0;
        for (uint32 n = 0; n < sphere_bot.num_verts; n++) {
            // bottom verts need to be scaled by radius, then have capsule.A added
            combined.vertices[curr_vert].position = (sphere_bot.vertices[n].position * collider->radius) + collider->C;
            combined.vertices[curr_vert++].normal = sphere_bot.vertices[n].normal;

            combined.indices[curr_idx++] = idx_offset + sphere_bot.indices[n];
        }
        idx_offset = curr_idx;
        for (uint32 n = 0; n < sphere_top.num_verts; n++) {
            // top verts need to be scaled by radius, then have capsule.B added
            combined.vertices[curr_vert].position = (sphere_top.vertices[n].position * collider->radius) + collider->C;
            combined.vertices[curr_vert++].normal =  sphere_top.vertices[n].normal;

            combined.indices[curr_idx++] = idx_offset + sphere_top.indices[n];
        }

        // now create geometry on the gpu for rendering
        ShaderDataType debug_attr[] = {ShaderDataType::Float3, ShaderDataType::Float3, ShaderDataType::None};
        renderer_create_mesh(geom, combined.num_verts, combined.vertices, combined.num_inds, combined.indices, debug_attr);
    }
}

void collision_grid_get_sector_capsule(collision_grid* grid, collision_sector* sector, laml::Vec3 P0, laml::Vec3 P1, real32 radius) {
    // find extents of the capsule in world coords
    laml::Vec3 radius_vec(radius);
    laml::Vec3 world_max_a = P0 + radius_vec;
    laml::Vec3 world_max_b = P1 + radius_vec;
    laml::Vec3 world_min_a = P0 - radius_vec;
    laml::Vec3 world_min_b = P1 - radius_vec;

    real32 max_x_world = laml::max(laml::Vec2(world_max_a.x, world_max_b.x));
    real32 max_y_world = laml::max(laml::Vec2(world_max_a.y, world_max_b.y));
    real32 max_z_world = laml::max(laml::Vec2(world_max_a.z, world_max_b.z));
    real32 min_x_world = laml::min(laml::Vec2(world_min_a.x, world_min_b.x));
    real32 min_y_world = laml::min(laml::Vec2(world_min_a.y, world_min_b.y));
    real32 min_z_world = laml::min(laml::Vec2(world_min_a.z, world_min_b.z));

    // convert to grid cells
    sector->x_max = (grid->num_x/2) + (int32)(floor((max_x_world - grid->origin.x) / grid->cell_size));
    sector->y_max = (grid->num_y/2) + (int32)(floor((max_y_world - grid->origin.y) / grid->cell_size));
    sector->z_max = (grid->num_z/2) + (int32)(floor((max_z_world - grid->origin.z) / grid->cell_size));

    sector->x_min = (grid->num_x/2) + (int32)(floor((min_x_world - grid->origin.x) / grid->cell_size));
    sector->y_min = (grid->num_y/2) + (int32)(floor((min_y_world - grid->origin.y) / grid->cell_size));
    sector->z_min = (grid->num_z/2) + (int32)(floor((min_z_world - grid->origin.z) / grid->cell_size));

    // bounds checking
    if (sector->x_min >= grid->num_x || sector->y_min >= grid->num_y || sector->z_min >= grid->num_z ||
        sector->x_max < 0 || sector->y_max < 0 || sector->z_max < 0) {
        // completely outside of the grid!
        sector->inside = false;
        return;
    }
    // clip the sector to the actual grid
    sector->x_max = (sector->x_max >= grid->num_x) ? (grid->num_x-1) : sector->x_max;
    sector->y_max = (sector->y_max >= grid->num_y) ? (grid->num_y-1) : sector->y_max;
    sector->z_max = (sector->z_max >= grid->num_z) ? (grid->num_z-1) : sector->z_max;

    sector->x_min = (sector->x_min < 0) ? 0 : sector->x_min;
    sector->y_min = (sector->y_min < 0) ? 0 : sector->y_min;
    sector->z_min = (sector->z_min < 0) ? 0 : sector->z_min;
    sector->inside = true;
}

void collision_grid_get_sector_sphere(collision_grid* grid, collision_sector* sector, laml::Vec3 C, real32 radius) {
    // find extents of the capsule in world coords
    laml::Vec3 radius_vec(radius);
    laml::Vec3 world_max = C + radius_vec;
    laml::Vec3 world_min = C - radius_vec;

    // convert to grid cells
    sector->x_max = (grid->num_x/2) + (int32)(floor((world_max.x - grid->origin.x) / grid->cell_size));
    sector->y_max = (grid->num_y/2) + (int32)(floor((world_max.y - grid->origin.y) / grid->cell_size));
    sector->z_max = (grid->num_z/2) + (int32)(floor((world_max.z - grid->origin.z) / grid->cell_size));

    sector->x_min = (grid->num_x/2) + (int32)(floor((world_min.x - grid->origin.x) / grid->cell_size));
    sector->y_min = (grid->num_y/2) + (int32)(floor((world_min.y - grid->origin.y) / grid->cell_size));
    sector->z_min = (grid->num_z/2) + (int32)(floor((world_min.z - grid->origin.z) / grid->cell_size));

    // bounds checking
    if (sector->x_min >= grid->num_x || sector->y_min >= grid->num_y || sector->z_min >= grid->num_z ||
        sector->x_max < 0 || sector->y_max < 0 || sector->z_max < 0) {
        // completely outside of the grid!
        sector->inside = false;
        return;
    }
    // clip the sector to the actual grid
    sector->x_max = (sector->x_max >= grid->num_x) ? (grid->num_x-1) : sector->x_max;
    sector->y_max = (sector->y_max >= grid->num_y) ? (grid->num_y-1) : sector->y_max;
    sector->z_max = (sector->z_max >= grid->num_z) ? (grid->num_z-1) : sector->z_max;

    sector->x_min = (sector->x_min < 0) ? 0 : sector->x_min;
    sector->y_min = (sector->y_min < 0) ? 0 : sector->y_min;
    sector->z_min = (sector->z_min < 0) ? 0 : sector->z_min;
    sector->inside = true;
}

uint32* collision_get_unique_triangles(collision_grid* grid, collision_sector* sector, memory_arena* arena, uint32* num_tris) {
    //if (sector == nullptr) {
    //    // get everything
    //    *num_tris = grid->num_triangles;
    //    return grid->triangles;
    //}

    *num_tris = 0;
    if (!sector->inside) return nullptr;

    // query sector of cells
    uint32 num_possible_tris = 0;
    for (int32 x = sector->x_min; x <= sector->x_max; x++) {
        for (int32 y = sector->y_min; y <= sector->y_max; y++) {
            for (int32 z = sector->z_min; z <= sector->z_max; z++) {
                collision_grid_cell* cell = &grid->cells[x][y][z];
                num_possible_tris += cell->num_surfaces;
            }
        }
    }

    if (num_possible_tris == 0) return nullptr;

    memory_index arena_save = arena->Used;
    uint32* unique_triangles = PushArray(arena, uint32, num_possible_tris);
    uint32* all_triangles = PushArray(arena, uint32, num_possible_tris);
    uint32 curr_tri_idx = 0;
    for (int32 x = sector->x_min; x <= sector->x_max; x++) {
        for (int32 y = sector->y_min; y <= sector->y_max; y++) {
            for (int32 z = sector->z_min; z <= sector->z_max; z++) {
                collision_grid_cell* cell = &grid->cells[x][y][z];
                for (uint32 n = 0; n < cell->num_surfaces; n++) {
                    all_triangles[curr_tri_idx++] = cell->surfaces[n];
                }
            }
        }
    }

    // all_triangles now has a lot of duplicates... find the unique ones
    uint32 num_unique_tris = 0;
    for (uint32 dupe_tri_idx = 0; dupe_tri_idx < num_possible_tris; dupe_tri_idx++) {
        uint32 curr_idx = all_triangles[dupe_tri_idx];

        bool32 found = false;
        for (uint32 unique_idx = 0; unique_idx < num_unique_tris; unique_idx++) {
            if (unique_triangles[unique_idx] == curr_idx) {
                found = true;
                break;
            }
        }

        if (!found) {
            unique_triangles[num_unique_tris++] = curr_idx;
        }
    }

    // 'shrink' the 'allocated' arrays down to the correct size
    arena->Used = arena_save + num_unique_tris*sizeof(uint32);

    *num_tris = num_unique_tris;
    return unique_triangles;
}