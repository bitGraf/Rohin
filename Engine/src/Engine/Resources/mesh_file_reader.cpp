#include "mesh_file_reader.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"
#include "Engine/Core/String.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Memory/MemoryUtils.h"

internal_func bool32 CheckTag(uint8* Buffer, const char* Token, size_t NumChars) {
    for (size_t n = 0; n < NumChars; n++) {
        if (*Buffer++ != Token[n]) {
            return false;
        }
    }
    return true;
}

internal_func bool32 CheckHeader(mesh_file_header* Header, uint64 Size) {
    if (Header->FileSize != Size) return false;
    if (!CheckTag(Header->Magic, "MESH", 4)) return false;

    // check version number
    if (Header->Version != 2) return false;

    return true;
}

mesh_file_result parse_mesh_file(const char* resource_file_name, mesh_file **mesh_file_data, memory_arena * arena) {
    *mesh_file_data = nullptr;

    char full_path[256];
    platform_get_full_resource_path(full_path, 256, resource_file_name);

    RH_DEBUG("Full filename: [%s]", full_path);

    file_handle file = platform_read_entire_file(full_path);
    if (!file.num_bytes) {
        RH_ERROR("Failed to read resource file");
        return mesh_file_result::error;
    }

    // process the file buffer now to get the mesh data!
    uint8* End = file.data + file.num_bytes;
    mesh_file_header* Header = AdvanceBuffer(&file.data, mesh_file_header, End);

    if (!CheckHeader(Header, file.num_bytes)) {
        Assert(!"Incorrect header\n");
        return mesh_file_result::error;
    }

    // allocate memory
    mesh_file *tmp = PushStruct(arena, mesh_file);
    memory_copy(&tmp->Header, Header, sizeof(mesh_file_header));

    // Read materials
    tmp->Materials = PushArray(arena, mesh_file_material, Header->NumPrims);
    for (int n = 0; n < Header->NumPrims; n++) {
        mesh_file_material_header * MaterialHeader = AdvanceBuffer(&file.data, mesh_file_material_header, End);
        memory_copy(&tmp->Materials[n].Header, MaterialHeader, sizeof(mesh_file_material_header));

        { // material name
            uint8 NameLen = *AdvanceBuffer(&file.data, uint8, End);
            unsigned char *Name = AdvanceBufferArray(&file.data, unsigned char, NameLen, End);

            tmp->Materials[n].Name.len = NameLen;
            memory_copy(tmp->Materials[n].Name.str, Name, NameLen);
            tmp->Materials[n].Name.str[NameLen] = 0;
        }

        if (MaterialHeader->Flag & 0x02) { // Diffuse Texture
            uint8 NameLen = *AdvanceBuffer(&file.data, uint8, End);
            unsigned char *Name = AdvanceBufferArray(&file.data, unsigned char, NameLen, End);

            tmp->Materials[n].DiffuseTexture.len = NameLen;
            memory_copy(tmp->Materials[n].DiffuseTexture.str, Name, NameLen);
            tmp->Materials[n].DiffuseTexture.str[NameLen] = 0;
        }
        if (MaterialHeader->Flag & 0x04) { // Normal Texture
            uint8 NameLen = *AdvanceBuffer(&file.data, uint8, End);
            unsigned char *Name = AdvanceBufferArray(&file.data, unsigned char, NameLen, End);

            tmp->Materials[n].NormalTexture.len = NameLen;
            memory_copy(tmp->Materials[n].NormalTexture.str, Name, NameLen);
            tmp->Materials[n].NormalTexture.str[NameLen] = 0;
        }
        if (MaterialHeader->Flag & 0x08) { // AMR Texture
            uint8 NameLen = *AdvanceBuffer(&file.data, uint8, End);
            unsigned char *Name = AdvanceBufferArray(&file.data, unsigned char, NameLen, End);

            tmp->Materials[n].AMRTexture.len = NameLen;
            memory_copy(tmp->Materials[n].AMRTexture.str, Name, NameLen);
            tmp->Materials[n].AMRTexture.str[NameLen] = 0;
        }
        if (MaterialHeader->Flag & 0x10) { // Emissive Texture
            uint8 NameLen = *AdvanceBuffer(&file.data, uint8, End);
            unsigned char *Name = AdvanceBufferArray(&file.data, unsigned char, NameLen, End);

            tmp->Materials[n].EmissiveTexture.len = NameLen;
            memory_copy(tmp->Materials[n].EmissiveTexture.str, Name, NameLen);
            tmp->Materials[n].EmissiveTexture.str[NameLen] = 0;
        }
    }

    // Read primitives
    tmp->Primitives = PushArray(arena, mesh_file_primitive, Header->NumPrims);
    for (int n = 0; n < Header->NumPrims; n++) {
        mesh_file_primitive_header *PrimHeader = AdvanceBuffer(&file.data, mesh_file_primitive_header, End);
        memory_copy(&tmp->Primitives[n].Header, PrimHeader, sizeof(mesh_file_primitive_header));

        uint32* Indices = AdvanceBufferArray(&file.data, uint32, PrimHeader->NumInds, End);
        tmp->Primitives[n].Indices = PushArray(arena, uint32, PrimHeader->NumInds);
        memory_copy(tmp->Primitives[n].Indices, Indices, PrimHeader->NumInds * sizeof(uint32));

        mesh_file_vertex* Vertices = AdvanceBufferArray(&file.data, mesh_file_vertex, PrimHeader->NumVerts, End);
        tmp->Primitives[n].Vertices = PushArray(arena, mesh_file_vertex, PrimHeader->NumVerts);
        memory_copy(tmp->Primitives[n].Vertices, Vertices, PrimHeader->NumVerts * sizeof(mesh_file_vertex));
    }

    uint8* EndTag = AdvanceBufferArray(&file.data, uint8, 4, End); // VERT
    if (!CheckTag(EndTag, "END", 4)) {
        Assert(!"Did not end up at the correct place in the file ;~;\n");
    }
    Assert(file.data == End);

    platform_free_file_data(&file);

    *mesh_file_data = tmp;

    return mesh_file_result::is_static;
}