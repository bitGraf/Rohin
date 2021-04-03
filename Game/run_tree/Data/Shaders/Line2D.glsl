#type vertex
#version 330 core
layout (location = 0) in float a_Position;

uniform vec2 r_verts[2];

uniform mat4 r_orthoProjection;

void main() {

    vec2 newPos = vec2(r_verts[gl_VertexID]);

    vec4 pos = r_orthoProjection * vec4(newPos.x, newPos.y, -1.0, 1.0);

    gl_Position = pos;
}

#type fragment
#version 330 core
out vec4 FragColor;

uniform vec4 r_Color;

void main() {
    FragColor = r_Color;
}