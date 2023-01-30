#type vertex
#version 430 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec3 a_Bitangent;
layout (location = 4) in vec2 vertTexCoord;
layout (location = 5) in ivec4 a_BoneIndices;
layout (location = 6) in vec4 a_BoneWeights;

out vec2 tex_coord;
out vec3 world_pos;
out vec3 world_normal;

layout (location = 1) uniform mat4 r_VP; // actually proj*view
layout (location = 2) uniform mat4 r_Transform;

void main() {
    tex_coord = vertTexCoord;
    world_pos = vec3(r_Transform * vec4(vertPos, 1.0));
    world_normal = vec3(r_Transform * vec4(a_Normal, 0.0));

    world_pos = vertPos;
    world_normal = a_Normal;

    gl_Position = r_VP * r_Transform * vec4(vertPos, 1.0);
}

#type fragment
#version 430 core

layout (location = 0) out vec4 FragColor;

in vec2 tex_coord;
in vec3 world_pos;
in vec3 world_normal;

layout (location = 3) uniform sampler2D u_texture;
layout (location = 4) uniform vec4 u_color;

void main() {
    float tex_scale = 1.0f;
    float sharpness = 1.0f;

    // find the uv coords for each axis
    vec2 uv_x = world_pos.zy / tex_scale;
    vec2 uv_y = world_pos.xz / tex_scale;
    vec2 uv_z = world_pos.xy / tex_scale;

    // sample the texture 3 times
    vec4 diff_x = texture(u_texture, uv_x);
    vec4 diff_y = texture(u_texture, uv_y);
    vec4 diff_z = texture(u_texture, uv_z);

    // use the normal to get the blend weight
    vec3 blend_weight = pow(abs(world_normal), vec3(sharpness));
    blend_weight = blend_weight / (blend_weight.x + blend_weight.y + blend_weight.z);

    vec4 triplanar_sample = diff_x*blend_weight.x + diff_y*blend_weight.y + diff_z*blend_weight.z;
    FragColor = (triplanar_sample) * u_color;
}