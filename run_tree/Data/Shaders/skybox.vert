#version 330 core
layout (location = 0) in vec3 vertPos;

out vec3 TexCoords;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main() {
    TexCoords = vertPos;
    vec4 pos = projectionMatrix * viewMatrix * vec4(vertPos, 1.0);
    gl_Position = pos.xyww;
}