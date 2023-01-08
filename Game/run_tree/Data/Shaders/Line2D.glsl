#type vertex
#version 430 core
layout (location = 0) in float a_Position;

layout (location = 1) uniform vec2 r_verts[2];

layout (location = 2) uniform mat4 r_orthoProjection;

void main() {

    vec2 newPos = vec2(r_verts[gl_VertexID]);

    vec4 pos = r_orthoProjection * vec4(newPos.x, newPos.y, -1.0, 1.0);

    gl_Position = pos;
}

#type fragment
#version 430 core
out vec4 FragColor;

layout (location = 3) uniform vec4 r_Color;

void main() {
    FragColor = r_Color;
}