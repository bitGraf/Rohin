#version 330 core
out vec4 FragColor;

in vec2 tex_coord;

uniform sampler2D tex;

const float gamma = 2.2;

void main() {
    vec3 color_in = texture(tex, tex_coord).rgb;
    
    vec3 gammaCorrected = pow(color_in, vec3(1.0/gamma));

    FragColor = vec4(gammaCorrected, 1);
}