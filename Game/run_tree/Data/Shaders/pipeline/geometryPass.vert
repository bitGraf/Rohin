#version 330 core
layout (location=0) in vec3 vertPos;
layout (location=1) in vec3 vertNorm;
layout (location=2) in vec2 vertTex;
layout (location=3) in vec4 vertTangent;
layout (location=4) in vec3 vertBitangent;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

// All in view-space
out vec3 pass_normal;
out vec3 pass_fragPos;
out mat3 pass_TBN;
out vec2 pass_tex;

void main() {
    vec4 viewPos = modelViewMatrix * vec4(vertPos, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix)));
    vec3 T = normalize(normalMatrix * vertTangent.xyz);
    vec3 N = normalize(normalMatrix * vertNorm);
	vec3 B = cross(N, T);

    pass_TBN = mat3(T, B, N);
    pass_normal = N;
    pass_fragPos = viewPos.xyz;
    pass_tex = vertTex;

    gl_Position = projectionMatrix * viewPos;
}