#version 330 core
layout (location=0) in vec3 vertPos;

uniform mat4 modelMatrix;
uniform mat4 projectionViewMatrix;

void main() {
    gl_Position = projectionViewMatrix * modelMatrix * vec4(vertPos, 1);
}