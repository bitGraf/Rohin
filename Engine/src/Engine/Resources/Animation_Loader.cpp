#include "Resource_Manager.h"

#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"
#include "Engine/Core/String.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Memory/Memory.h"

#include "Engine/Renderer/Renderer.h"

#include "Engine/Resources/Filetype/anim_file_reader.h"

bool32 resource_load_animation(const char* resource_file_name, resource_animation* anim) {
    memory_arena* arena = resource_get_arena();

    anim_file* file_data;
    bool32 res = parse_anim_file(resource_file_name, &file_data, arena);
    if (res == false) {
        RH_ERROR("Failed to parse data from .anim file.");
        return false;
    }

    anim->name = "<anim_name>";
    anim->num_bones = file_data->Header.NumBones;
    anim->num_samples = file_data->Header.NumSamples;
    anim->frame_rate = file_data->Header.FrameRate;
    anim->length = anim->num_samples / anim->frame_rate;

    anim->bones = PushArray(arena, bone_anim, anim->num_bones);
    for (uint32 n = 0; n < anim->num_bones; n++) {
        anim->bones[n].translation = file_data->bones[n].translation;
        anim->bones[n].rotation    = file_data->bones[n].rotation;
        anim->bones[n].scale       = file_data->bones[n].scale;
    }

    return true;
}