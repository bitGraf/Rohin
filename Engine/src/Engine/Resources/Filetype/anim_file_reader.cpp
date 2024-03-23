#include "anim_file_reader.h"

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

internal_func bool32 CheckHeader(anim_file_header* Header, uint64 Size) {
    if (Header->FileSize != Size) return false;
    if (!CheckTag(Header->Magic, "ANIM", 4)) return false;

    // check version number
    if (Header->Version != 1) return false;

    return true;
}

bool32 parse_anim_file(const char* resource_file_name, anim_file **anim_file_data, memory_arena * arena) {
    *anim_file_data = nullptr;

    char full_path[256];
    platform_get_full_resource_path(full_path, 256, resource_file_name);

    RH_TRACE("Full filename: [%s]", full_path);

    file_handle file = platform_read_entire_file(full_path);
    if (!file.num_bytes) {
        RH_ERROR("Failed to read resource file");
        return false;
    }

    // process the file buffer now to get the mesh data!
    uint8* End = file.data + file.num_bytes;
    anim_file_header* Header = AdvanceBuffer(&file.data, anim_file_header, End);

    if (!CheckHeader(Header, file.num_bytes)) {
        AssertMsg(false, "Incorrect .anim file header!");
        return false;
    }

    // allocate memory
    anim_file *tmp = PushStruct(arena, anim_file);
    memory_copy(&tmp->Header, Header, sizeof(anim_file_header));

    // read skeleton if skinned
    anim_file_skeleton_header* SkeletonHeader = AdvanceBuffer(&file.data, anim_file_skeleton_header, End);
    memory_copy(&tmp->Skeleton.Header, SkeletonHeader, sizeof(anim_file_skeleton_header));

    // pull skeleton heirarchy
    tmp->Skeleton.Bones = PushArray(arena, anim_file_bone, SkeletonHeader->NumBones);
    for (uint32 b = 0; b < SkeletonHeader->NumBones; b++) {

        uint16* bone_idx = AdvanceBuffer(&file.data, uint16, End);
        tmp->Skeleton.Bones[b].bone_idx = *bone_idx;

        int16* parent_idx = AdvanceBuffer(&file.data, int16, End);
        tmp->Skeleton.Bones[b].parent_idx = *parent_idx;
    }

    // get sampled animation frames
    tmp->bones = PushArray(arena, anim_file_bone_anim, SkeletonHeader->NumBones);
    for (uint32 b = 0; b < SkeletonHeader->NumBones; b++) {
        struct __tmp_t {
            unsigned char MAGIC[4];
        };
        __tmp_t* tmp2 = AdvanceBuffer(&file.data, __tmp_t, End);
        if (!CheckTag(tmp2->MAGIC, "BONE", 4)) {
            break;
        }

        laml::Vec3* translation = AdvanceBufferArray(&file.data, laml::Vec3, Header->NumSamples, End);
        tmp->bones[b].translation = PushArray(arena, laml::Vec3, Header->NumSamples);
        memory_copy(tmp->bones[b].translation, translation, sizeof(laml::Vec3)*Header->NumSamples);

        laml::Quat* rotation = AdvanceBufferArray(&file.data, laml::Quat, Header->NumSamples, End);
        tmp->bones[b].rotation = PushArray(arena, laml::Quat, Header->NumSamples);
        memory_copy(tmp->bones[b].rotation, rotation, sizeof(laml::Quat)*Header->NumSamples);

        laml::Vec3* scale = AdvanceBufferArray(&file.data, laml::Vec3, Header->NumSamples, End);
        tmp->bones[b].scale = PushArray(arena, laml::Vec3, Header->NumSamples);
        memory_copy(tmp->bones[b].scale, scale, sizeof(laml::Vec3)*Header->NumSamples);
    }

    uint8* EndTag = AdvanceBufferArray(&file.data, uint8, 4, End); // VERT
    if (!CheckTag(EndTag, "END", 4)) {
        AssertMsg(false, "Did not end up at the correct place in the file ;~;\n");
    }
    AssertMsg(file.data == End, "Not at the end of the file");

    platform_free_file_data(&file);

    *anim_file_data = tmp;

    return true;
}