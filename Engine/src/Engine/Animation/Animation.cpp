#include "Animation.h"

bool32 sample_animation_at_time(const resource_skinned_mesh* mesh,  const resource_animation* anim, real32 time, laml::Mat4* model_matrices) {
    uint16 num_samples = anim->num_samples;
    while (time > anim->length) {
        time -= anim->length;
    }

    const real32 sample_time = 1.0f / anim->frame_rate;
    uint16 sample_idx = (uint16)(time / sample_time); // just get the nearest index

    // 1. First! calculate the local transform for each bone
    uint16 num_bones = anim->num_bones;
    for (uint16 n = 0; n < num_bones; n++) {
        const bone_anim& bone = anim->bones[n];

        // 1.a build a transform from the TRS of the bone. Right now, just do 'nearest' interpolation (i.e. None).
        //     In the future, interpolate b/w frames and also between animations.
        laml::Vec3 translation = bone.translation[sample_idx];
        laml::Quat rotation    = bone.rotation[sample_idx];
        laml::Vec3 scale       = bone.scale[sample_idx];

        laml::transform::create_transform(model_matrices[n], rotation, translation, scale);

        // 1.b For each model_matrix, calculate its actual model-space transform by using 
        //     the inv_bind_pose matrices and parent_idx
        //     Note: we can do this in the same for-loop as above, since the bones 
        //     are ordered such that a bones parent ALWAYS occurs earlier in the list.
        if (mesh->skeleton.bones[n].parent_idx == -1) {
            // do nothing, no parent
        } else {
            // pre-multiply by the parent's local_transform
            int32 parent_idx = mesh->skeleton.bones[n].parent_idx;
            model_matrices[n] = laml::mul(model_matrices[parent_idx], model_matrices[n]);
        }
    }

    // 2. Now, get the difference transform by post-multiplying by the inv_bind_pose.
    for (uint16 n = 0; n < num_bones; n++) {
        const laml::Mat4& inv_bind_pose = mesh->skeleton.bones[n].inv_model_matrix;

        model_matrices[n] = laml::mul(model_matrices[n], inv_bind_pose);
    }

    return true;
}