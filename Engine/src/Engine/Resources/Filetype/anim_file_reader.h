#pragma once

#include "Engine/Resources/Resource_Types.h"

struct anim_file_header {
    unsigned char Magic[4];
    uint32 FileSize;
    uint32 Version;
    uint32 Flag;
    uint64 Timestamp;
    uint16 NumBones;
    uint16 NumSamples;
    real32 FrameRate;
};
struct anim_file_string {
    uint8 len;
    char str[FILE_MAX_STRING_LENGTH];
};

struct anim_file_skeleton_header {
    unsigned char Magic[4];
    uint16 NumBones;
};
struct anim_file_bone {
    uint16 bone_idx;
    int16  parent_idx;
};
struct anim_file_skeleton {
    anim_file_skeleton_header Header;

    anim_file_bone* Bones;
};
struct anim_file_bone_anim {
    laml::Vec3* translation;
    laml::Quat* rotation;
    laml::Vec3* scale;
};

struct anim_file {
    anim_file_header Header;
    anim_file_skeleton Skeleton;

    uint16 num_bones;
    uint16 num_samples;
    real32 frame_rate;
    real32 length;

    anim_file_bone_anim* bones;
};

bool32 parse_anim_file(const char* resource_file_name, anim_file **file_data, memory_arena* arena);