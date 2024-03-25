#type vertex
#version 430 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

out vec2 texcoord;

layout (location = 1) uniform vec4  r_rect; // in pixels
layout (location = 2) uniform float r_screen_width;
layout (location = 3) uniform float r_screen_height;

void main() {
    float ndc_x = (2.0 * r_rect.x / r_screen_width)  - 1.0;
    float ndc_y = (2.0 * r_rect.y / r_screen_height) - 1.0;
    float scale_x = 2.0 * r_rect.z / r_screen_width;
    float scale_y = 2.0 * r_rect.w / r_screen_height;
    vec4 position = vec4(scale_x*a_Position.x + ndc_x, scale_y*a_Position.y + ndc_y, 0.0, 1.0);
	gl_Position = position;

	texcoord = a_TexCoord;
}

#type fragment
#version 430 core
out vec4 FragColor;

in vec2 texcoord;

layout (location = 4) uniform sampler2D u_texture;

void main() {
    vec4 color = texture(u_texture, texcoord);

    FragColor = vec4(color.r, color.r, color.r, 1.0);
}