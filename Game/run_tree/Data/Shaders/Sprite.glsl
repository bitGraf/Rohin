#type vertex
#version 430 core
layout (location = 0) in vec2 a_Position;

out vec2 pass_uv;

layout (location = 1) uniform vec4 r_transform;
layout (location = 2) uniform vec4 r_transformUV;

layout (location = 3) uniform mat4 r_orthoProjection;

void main() {

    vec2 newPos = vec2(r_transform.x   * a_Position.x, r_transform.y   * a_Position.y) + r_transform.zw;
    vec2 newUV  = vec2(r_transformUV.x * a_Position.x, r_transformUV.y * (1-a_Position.y)) + vec2(r_transformUV.z, r_transformUV.w);

    vec4 pos = r_orthoProjection * vec4(newPos.x, newPos.y, -1.0, 1.0);

    gl_Position = pos;
    pass_uv = newUV;
}

#type fragment
#version 430 core
out vec4 FragColor;

in vec2 pass_uv;

layout (location = 4) uniform sampler2D r_spriteTex;
layout (location = 5) uniform vec3 r_textColor;

void main() {
    vec4 frag = texture(r_spriteTex, pass_uv);

    FragColor = frag * vec4(r_textColor,1);
}