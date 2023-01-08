#type vertex
#version 430 core
layout (location = 0) in float a_Position;

layout (location = 1) uniform vec3 r_verts[2];

layout (location = 3) uniform mat4 r_View;
layout (location = 4) uniform mat4 r_Projection;

void main() {
    gl_Position = r_Projection * r_View * vec4(r_verts[gl_VertexID], 1.0);
}

#type fragment
#version 430 core
out vec4 FragColor;

layout (location = 5) uniform vec4 r_Color;

void main() {
    FragColor = r_Color;
}