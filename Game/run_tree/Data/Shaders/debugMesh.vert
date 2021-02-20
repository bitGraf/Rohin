#version 330 core
layout (location=0) in vec3 vertPos;
layout (location=1) in vec3 vertNorm;

uniform mat4 modelMatrix;
uniform mat4 projectionViewMatrix;

out vec3 pass_normal;
out vec3 pass_fragPos;

void main() {
    pass_fragPos = vec3(modelMatrix * vec4(vertPos, 1.0));
    gl_Position = projectionViewMatrix * modelMatrix * vec4(vertPos, 1.0);

    
    vec3 N = normalize(vec3(modelMatrix*vec4(vertNorm, 0.0)));
    pass_normal = N;
}