#version 330 core
layout (location = 0) in vec2 vertPos;

out vec2 tex_coord;
out mat4 lightSpaceMatrix;

uniform mat4 lightProjectionViewMatrix;

void main() {
    tex_coord = vec2((vertPos.x+1)/2, (vertPos.y+1)/2);
    gl_Position = vec4(vertPos, 0, 1);

    lightSpaceMatrix = lightProjectionViewMatrix;
}