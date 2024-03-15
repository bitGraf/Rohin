#pragma once

#include "Engine/Resources/Resource_Types.h"

RHAPI bool32 sample_animation_at_time(const resource_skinned_mesh* mesh,  const resource_animation* anim, real32 time, laml::Mat4* model_matrices);