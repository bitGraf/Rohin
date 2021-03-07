#version 330 core
layout (location=0) in float vertPos;

uniform mat4 projectionViewMatrix;

uniform vec3 vertexA;
uniform vec3 vertexB;
uniform vec3 colorA;
uniform vec3 colorB;

out vec3 pass_color;

void main() {
    vec3 p = vertexA*(1-vertPos) + vertexB*vertPos;
    vec3 c = colorA*(1-vertPos)  + colorB*vertPos;
    
    pass_color = c;
    gl_Position = projectionViewMatrix * vec4(p, 1);
}