#version 330 core
out vec4 FragColor;

in vec2 tex_coord;

uniform sampler2D tex;

void main() {
    const float gamma = 2.2;
    vec3 hdrColor = texture(tex, tex_coord).rgb;
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    mapped = pow(mapped, vec3(1.0/gamma));

    FragColor = vec4(mapped, 1);
}