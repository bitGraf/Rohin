#version 330 core
layout (location=0) in vec3 vertPos;
//layout (location=1) in vec4 vertColor;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 pass_fragPos; //worldSpace
//out vec4 pass_color;

void main() {
    pass_fragPos = vertPos;
    //pass_color = vertColor;

    gl_Position = projectionMatrix * viewMatrix * vec4(pass_fragPos, 1);
}