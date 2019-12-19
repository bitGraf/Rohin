#version 330 core
layout (location=0) in vec3 vertPos;
layout (location=1) in vec3 vertNorm;
layout (location=2) in vec2 vertTex;
layout (location=3) in vec4 vertTangent;
layout (location=4) in vec3 vertBitangent;

uniform mat4 modelMatrix;
uniform mat4 projectionViewMatrix;

out vec3 pass_normal;
out vec3 pass_fragPos;
out vec2 pass_tex;
out mat3 pass_TBN;

void main() {
    pass_fragPos = vec3(modelMatrix * vec4(vertPos, 1.0));
    gl_Position = projectionViewMatrix * modelMatrix * vec4(vertPos, 1.0);

    vec3 T = normalize(vec3(modelMatrix*vec4(vec3(vertTangent), 0.0)));
    vec3 N = normalize(vec3(modelMatrix*vec4(vertNorm, 0.0)));
	vec3 B = cross(N, T);
    pass_TBN = mat3(T, B, N);
    pass_normal = N;

    pass_tex = vertTex;
}