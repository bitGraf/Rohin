#type vertex
#version 430 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertTexCoord;

out vec2 tex_coord;

uniform mat4 r_VP;
uniform mat4 r_Transform;

void main() {
    tex_coord = vertTexCoord;
    gl_Position = r_VP * r_Transform * vec4(vertPos, 1.0);
}

#type fragment
#version 430 core

layout (location = 0) out vec4 FragColor;

in vec2 tex_coord;

uniform sampler2D u_texture;
uniform vec4 u_color;

void main() {
    FragColor = texture(u_texture, tex_coord) * u_color;
}