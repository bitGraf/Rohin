#pragma once

#include "Engine/Renderer/Render_Types.h"

struct ShaderUniform {
    uint32 Location;
};

typedef ShaderUniform ShaderUniform_int;

typedef ShaderUniform ShaderUniform_float;
typedef ShaderUniform ShaderUniform_vec2;
typedef ShaderUniform ShaderUniform_vec3;
typedef ShaderUniform ShaderUniform_vec4;
typedef ShaderUniform ShaderUniform_mat3;
typedef ShaderUniform ShaderUniform_mat4;
typedef ShaderUniform ShaderUniform_sampler2D;
typedef ShaderUniform ShaderUniform_samplerCube;

struct ShaderUniform_Light {
    uint32 Location;

    ShaderUniform_vec3 Position;
    ShaderUniform_vec3 Direction;
    ShaderUniform_vec3 Color;
    ShaderUniform_float Strength;
    ShaderUniform_float Inner;
    ShaderUniform_float Outer;
};