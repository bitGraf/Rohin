#version 330 core
out vec4 FragColor;

in vec2 tex_coord;

uniform sampler2D tex;

void main() {
    vec3 hdrColor = texture(tex, tex_coord).rgb;

    vec3 toneMapped = hdrColor / (hdrColor + vec3(1.0));

    FragColor = vec4(toneMapped, 1);
}