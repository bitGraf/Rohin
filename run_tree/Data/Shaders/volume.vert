#version 330 core
layout (location=0) in vec3 vertPos;
layout (location=1) in vec3 vertNorm;
layout (location=2) in vec2 vertTex;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 lightProjectionMatrix;
uniform mat4 lightViewMatrix;

out vec3 pass_normal;
out vec3 pass_fragPos;
out vec4 pass_fragPosLightSpace;
out vec2 pass_tex;
out mat4 pass_lightSpaceMatrix;

void main() {
    pass_fragPos = vec3(modelMatrix * vec4(vertPos, 1.0));
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertPos, 1.0);

    pass_normal = normalize(vec3(modelMatrix*vec4(vertNorm, 0.0)));

    pass_tex = vertTex;

    pass_fragPosLightSpace = lightProjectionMatrix * lightViewMatrix * vec4(pass_fragPos, 1.0);
    pass_lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;
}