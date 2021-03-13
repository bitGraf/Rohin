#type vertex
#version 330 core
layout (location = 0) in vec2 a_Position;

out vec2 pass_uv;

uniform vec4 r_transform;
uniform vec4 r_transformUV;

uniform mat4 r_orthoProjection;

void main() {

    vec2 newPos = vec2(r_transform.x   * a_Position.x, r_transform.y   * a_Position.y) + r_transform.zw;
    vec2 newUV  = vec2(r_transformUV.x * a_Position.x, r_transformUV.y * a_Position.y) + vec2(r_transformUV.z, r_transformUV.w);

    vec4 pos = r_orthoProjection * vec4(newPos.x, newPos.y, -1.0, 1.0);

    gl_Position = pos;
    pass_uv = newUV;
}

#type fragment
#version 330 core
out vec4 FragColor;

in vec2 pass_uv;

uniform sampler2D r_fontTex;
uniform vec3 r_textColor;

void main() {
    float val  = texture(r_fontTex, pass_uv).r;
    vec4 text = vec4(vec3(1), val);

    FragColor = text * vec4(r_textColor,1); // make color alpha an input?
}