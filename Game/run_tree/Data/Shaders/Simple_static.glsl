#type vertex
#version 430 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec3 a_Binormal;
layout (location = 4) in vec2 a_TexCoord;

out vec2 tex_coord;

layout (location = 1) uniform mat4 r_VP;
layout (location = 2) uniform mat4 r_Transform;

void main() {
    tex_coord = a_TexCoord;
    gl_Position = r_VP * r_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 430 core

layout (location = 0) out vec4 FragColor;

in vec2 tex_coord;

layout (location = 3) uniform sampler2D u_texture;
layout (location = 4) uniform vec4 u_color;

void main() {
    FragColor = texture(u_texture, tex_coord) * u_color;
}