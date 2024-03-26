#include "mesh_file_reader.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"
#include "Engine/Core/String.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Memory/Memory.h"

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
    if (Header->Version != 4) return false;

    return true;
}

mesh_file_result parse_mesh_file(const char* resource_file_name, mesh_file **mesh_file_data, memory_arena * arena) {
    *mesh_file_data = nullptr;

    char full_path[256];
    platform_get_full_resource_path(full_path, 256, resource_file_name);

    RH_TRACE("Full filename: [%s]", full_path);

    file_handle file = platform_read_entire_file(full_path);
    if (!file.num_bytes) {
        RH_ERROR("Failed to read resource file");
        return mesh_file_result::error;
    }

    // process the file buffer now to get the mesh data!
    uint8* End = file.data + file.num_bytes;
    mesh_file_header* Header = AdvanceBuffer(&file.data, mesh_file_header, End);

    if (!CheckHeader(Header, file.num_bytes)) {
        AssertMsg(false, "Incorrect .mesh file header!");
        return mesh_file_result::error;
    }

    // allocate memory
    mesh_file *tmp = PushStruct(arena, mesh_file);
    memory_copy(&tmp->Header, Header, sizeof(mesh_file_header));

    bool32 is_skinned = Header->Flag & 0x01;

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

        uint64 vert_size = is_skinned ? skinned_vert_size : static_vert_size;

        if (PrimHeader->PrimType == 2) // LINE primitive!
            vert_size = line_vert_size;

        void* VerticesFromFile = AdvanceBufferSize_(&file.data, (PrimHeader->NumVerts)*(vert_size), End);
        tmp->Primitives[n].Vertices = PushSize_(arena, (PrimHeader->NumVerts)*(vert_size));
        tmp->Primitives[n].VertexAttribsSize = vert_size;
        memory_copy(tmp->Primitives[n].Vertices, VerticesFromFile, PrimHeader->NumVerts * vert_size);
    }

    // read skeleton if skinned
    if (is_skinned) {
        mesh_file_skeleton_header* SkeletonHeader = AdvanceBuffer(&file.data, mesh_file_skeleton_header, End);
        memory_copy(&tmp->Skeleton.Header, SkeletonHeader, sizeof(mesh_file_skeleton_header));

        tmp->Skeleton.Bones = PushArray(arena, mesh_file_bone, SkeletonHeader->NumBones);
        for (uint32 b = 0; b < SkeletonHeader->NumBones; b++) {

            uint32* bone_idx = AdvanceBuffer(&file.data, uint32, End);
            tmp->Skeleton.Bones[b].bone_idx = *bone_idx;

            int32* parent_idx = AdvanceBuffer(&file.data, int32, End);
            tmp->Skeleton.Bones[b].parent_idx = *parent_idx;

            real32* debug_length = AdvanceBuffer(&file.data, real32, End);
            tmp->Skeleton.Bones[b].debug_length = *debug_length;

            laml::Mat4* local_matrix = AdvanceBuffer(&file.data, laml::Mat4, End);
            tmp->Skeleton.Bones[b].local_matrix = *local_matrix;

            laml::Mat4* inv_model_matrix = AdvanceBuffer(&file.data, laml::Mat4, End);
            tmp->Skeleton.Bones[b].inv_model_matrix = *inv_model_matrix;


            // bone name
            uint8 NameLen = *AdvanceBuffer(&file.data, uint8, End);
            unsigned char *Name = AdvanceBufferArray(&file.data, unsigned char, NameLen, End);

            tmp->Skeleton.Bones[b].name.len = NameLen;
            memory_copy(tmp->Skeleton.Bones[b].name.str, Name, NameLen);
            tmp->Skeleton.Bones[b].name.str[NameLen] = 0;
        }
    }

    uint8* EndTag = AdvanceBufferArray(&file.data, uint8, 4, End); // VERT
    if (!CheckTag(EndTag, "END", 4)) {
        AssertMsg(false, "Did not end up at the correct place in the file ;~;");
    }
    AssertMsg(file.data == End, "Not at the end of the file!");

    platform_free_file_data(&file);

    *mesh_file_data = tmp;

    return is_skinned ? mesh_file_result::is_skinned : mesh_file_result::is_static;
}