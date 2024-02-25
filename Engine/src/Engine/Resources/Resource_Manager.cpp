#include "Resource_Manager.h"

#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Core/Asserts.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/String.h"

#include "Engine/Platform/Platform.h"
#include "Engine/Renderer/Renderer.h"

global_variable memory_arena* resource_arena;

bool32 resource_init(memory_arena* arena) {
    resource_arena = arena;

    resource_arena->Size;

    return true;
}

void resource_shutdown() {
    ResetArena(resource_arena);
}

memory_arena* resource_get_arena() {
    Assert(resource_arena);

    return resource_arena;
}


RHAPI bool32 resource_load_debug_mesh_data(const char* resource_file_name, debug_geometry* geom) {
    char full_path[256];
    platform_get_full_resource_path(full_path, 256, resource_file_name);

    RH_DEBUG("Full filename: [%s]", full_path);

    file_handle file = platform_read_entire_file(full_path);
    if (!file.num_bytes) {
        RH_ERROR("Failed to read resource file");
        return false;
    }

    memory_arena* arena = resource_get_arena();

    // process the file buffer now to get the mesh data!
    uint8* End = file.data + file.num_bytes;
    uint8* header = AdvanceBufferArray(&file.data, uint8, 80, End);

    uint32 num_tris = *AdvanceBuffer(&file.data, uint32, End);
    RH_DEBUG("%d triangles", num_tris);
    geom->num_verts = num_tris * 3;
    geom->num_inds = num_tris * 3;
    geom->vertices = PushArray(arena, debug_geometry::debug_vertex, geom->num_verts);
    geom->indices = PushArray(arena, uint32, geom->num_inds);

    struct stl_triangle {
        laml::Vec3 normal;
        laml::Vec3 v1;
        laml::Vec3 v2;
        laml::Vec3 v3;
        uint16 attribute_byte_count;
        uint16 ___padding;
    };
    const uint64 struct_size = 50; // actual size of struct is 52 bytes due to padding

    uint32 curr_vert = 0;
    uint32 curr_index = 0;
    for (uint32 n = 0; n < num_tris; n++) {
        stl_triangle* triangle = (stl_triangle*)AdvanceBufferSize_(&file.data, struct_size, End);

#if 0
        RH_TRACE("Triangle #%d\n         "
                 "normal: [%.1f,%.1f,%.1f]\n         "
                 "v1:     [%.1f,%.1f,%.1f]\n         "
                 "v2:     [%.1f,%.1f,%.1f]\n         "
                 "v3:     [%.1f,%.1f,%.1f]\n         "
                 "attribute_byte_count = %d",
                 n, triangle->normal.x, triangle->normal.y, triangle->normal.z,
                 triangle->v1.x, triangle->v1.y, triangle->v1.z,
                 triangle->v2.x, triangle->v2.y, triangle->v2.z,
                 triangle->v3.x, triangle->v3.y, triangle->v3.z, triangle->attribute_byte_count);
#endif

        geom->vertices[curr_vert].position = triangle->v1;
        geom->vertices[curr_vert++].normal = triangle->normal;
        geom->vertices[curr_vert].position = triangle->v2;
        geom->vertices[curr_vert++].normal = triangle->normal;
        geom->vertices[curr_vert].position = triangle->v3;
        geom->vertices[curr_vert++].normal = triangle->normal;

        // gcc complains about   indices[curr_index] = curr_index++;  >.>
        geom->indices[curr_index] = curr_index;curr_index++;
        geom->indices[curr_index] = curr_index;curr_index++;
        geom->indices[curr_index] = curr_index;curr_index++;
    }

    return true;
}

RHAPI bool32 resource_load_debug_mesh_into_geometry(const char* resource_file_name, render_geometry* geom) {
    debug_geometry data;
    if (!resource_load_debug_mesh_data(resource_file_name, &data)) {
        return false;
    }

    // now create geometry on the gpu for rendering
    ShaderDataType debug_attr[] = {ShaderDataType::Float3, ShaderDataType::Float3, ShaderDataType::None};
    renderer_create_mesh(geom, data.num_verts, data.vertices, data.num_inds, data.indices, debug_attr);

    return true;
}