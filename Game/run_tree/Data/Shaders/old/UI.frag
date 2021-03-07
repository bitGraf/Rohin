#version 330 core
out vec4 FragColor;

in vec2 pass_uv;

uniform sampler2D tex;
uniform float factor;

void main() {
    vec4 texColor = texture(tex, pass_uv);

    float alpha = texColor.a * factor;
    vec3 color = texColor.rgb;

    FragColor = vec4(color, alpha);
}