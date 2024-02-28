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

layout (location = 1) uniform mat4 r_VP; // actually proj*view
layout (location = 2) uniform mat4 r_Transform;

void main() {
    tex_coord = vertTexCoord;
    gl_Position = r_VP * r_Transform * vec4(vertPos, 1.0);
}

#type fragment
#version 430 core

layout (location = 0) out vec4 FragColor;

in vec2 tex_coord;

layout (location = 3) uniform sampler2D u_texture;
layout (location = 4) uniform vec3 u_color;

void main() {
    FragColor = texture(u_texture, tex_coord) * vec4(u_color, 1.0);
}