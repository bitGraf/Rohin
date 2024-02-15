#include "Resource_Manager.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"
#include "Engine/Core/String.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Memory/MemoryUtils.h"

#include "Engine/Renderer/Renderer.h"

#include "Engine/Collision/Collision.h"

#include <cstring>

static bool32 CheckTag(uint8* Buffer, const char* Token, size_t NumChars) {
    for (size_t n = 0; n < NumChars; n++) {
        if (*Buffer++ != Token[n]) {
            return false;
        }
    }
    return true;
}

struct level_file_header {
    unsigned char Magic[4];
    uint32 FileSize;
    uint32 Version;
    uint32 Flag;
    uint64 Timestamp;
    uint16 NumMeshes;
    uint16 NumMaterials;
    uint32 PADDING; // <- To bring struct size to 32 bytes!
};

struct level_file_entry {
    laml::Mat4 Transform;
    bool32 IsCollider;
    
    uint8 name_len;
    char *name;

    uint8 mesh_name_len;
    char *mesh_name;
};

struct level_file {
    uint32 num_entries;
    level_file_entry * entries;

    /*
    uint32 num_render_geom;
    uint32 num_collision_geom;

    triangle_geometry * render_geom;
    triangle_geometry * collider_geom;
    */
};

bool32 resource_load_level_file(const char* resource_file_name, collision_grid* grid, triangle_geometry *collider_geom) {
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
    level_file_header* Header = AdvanceBuffer(&file.data, level_file_header, End);

    level_file *level = PushStruct(arena, level_file);
    level->num_entries = (uint32)Header->NumMeshes;
    level->entries = PushArray(arena, level_file_entry, level->num_entries);

    uint32 num_renders = 0;
    uint32 num_colliders = 0;
    for (int32 mesh_no = 0; mesh_no < Header->NumMeshes; mesh_no++) {
        level_file_entry* entry = &level->entries[mesh_no];

        /*
        FILESIZE += fwrite(&mesh.transform.c_11, sizeof(real32), 16, fid) * sizeof(real32);
        FILESIZE += fwrite(&mesh.is_collider, sizeof(bool32), 1, fid) * sizeof(bool32);
        FILESIZE += write_string(fid, mesh.name);
        FILESIZE += write_string(fid, mesh.mesh_name);
        */
        real32 *Transform = AdvanceBufferArray(&file.data, real32, 16, End);
        uint32 IsCollider = *AdvanceBuffer(&file.data, uint32, End);

        entry->IsCollider = (bool32)(IsCollider);
        entry->Transform = laml::Mat4(Transform);

        if (IsCollider) {
            num_colliders++;
        } else {
            num_renders++;
        }

        {
            uint8 NameLen = *AdvanceBuffer(&file.data, uint8, End);
            unsigned char *Name = AdvanceBufferArray(&file.data, unsigned char, NameLen, End);
            entry->name_len = NameLen;
            entry->name = PushArray(arena, char, (NameLen) + 1);
            // TODO: better string copy code lol
            for (uint16 nn = 0; nn < NameLen; nn++) {
                entry->name[nn] = Name[nn];
            }
            entry->name[NameLen] = 0;
        }

        {
            uint8 MeshNameLen = *AdvanceBuffer(&file.data, uint8, End);
            unsigned char *MeshName = AdvanceBufferArray(&file.data, unsigned char, MeshNameLen, End);
            entry->mesh_name_len = MeshNameLen;
            entry->mesh_name = PushArray(arena, char, (MeshNameLen) + 1);
            // TODO: better string copy code lol
            for (uint16 nn = 0; nn < MeshNameLen; nn++) {
                entry->mesh_name[nn] = MeshName[nn];
            }
            entry->mesh_name[MeshNameLen] = 0;
        }

        RH_TRACE(" Entry %d: [%s] -> [%s]", mesh_no, entry->name, entry->mesh_name);

        bool done = true;
    }

    uint8* EndTag = AdvanceBufferArray(&file.data, uint8, 4, End); // VERT
    if (!CheckTag(EndTag, "END", 4)) {
        Assert(!"Did not end up at the correct place in the file ;~;\n");
    }
    Assert(file.data == End);

    platform_free_file_data(&file);

    // build path to level files
    char level_path[256] = { 0 };
    char* s = level_path;
    for (const char* p = resource_file_name; *p; p++, s++) {
        *s = *p;
    }
    while ((*s != '\\') && (*s != '/')) {
        s--;
    }
    s++;
    {
        char* collider_folder = "collision_meshes/";
        for (char* p = collider_folder; *p; p++, s++) {
            *s = *p;
        }
    }
    char* collider_folder = s;

    // Now that the entries have been read rfom the file, start parsing them into render/collision objects
    collision_create_grid(arena, grid, {25.0f, -0.1f, -5.0f}, 0.5f, 256, 16, 256);
    uint32 count_collider = 0;
    for (uint32 n = 0; n < level->num_entries; n++) {
        level_file_entry * entry = &level->entries[n];

        if (entry->IsCollider) {
            count_collider++;
            if (count_collider == 1) { //TMP: only want to load one collider, need to refactor things.
                continue;
            }
            s = collider_folder;
            for (char* p = entry->mesh_name; *p; p++, s++) {
                *s = *p;
            }
            {
                char* ext = ".mesh";
                for (char* p = ext; *p; p++, s++) {
                    *s = *p;
                }
            }

            resource_load_mesh_file_for_level(level_path, collider_geom, grid, entry->Transform);
        }
    }
    collision_grid_finalize(arena, grid);

    return true;
}