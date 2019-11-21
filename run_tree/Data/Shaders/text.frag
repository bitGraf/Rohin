#version 330 core
out vec4 FragColor;

in vec2 pass_uv;

uniform sampler2D fontTex;
uniform vec4 textColor;

void main() {
    float val  = texture(fontTex, pass_uv).r;
    vec4 text = vec4(vec3(1), val);

    FragColor = text * textColor;
}