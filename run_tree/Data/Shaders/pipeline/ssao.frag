#version 330 core
layout (location = 0) out float FragColor;

in vec2 tex_coord;

uniform sampler2D TargetPos;
uniform sampler2D Target1; // for normal
uniform sampler2D texNoise;

uniform mat4 projectionMatrix;

const int MAX_KERNEL_SIZE = 32;
uniform vec3 Kernel[MAX_KERNEL_SIZE];
const float radius = 0.5;
const float bias = 0.025;
uniform vec2 noiseScale;

void main() {
    vec3 fragPos = texture(TargetPos, tex_coord).xyz;
    vec3 normal = normalize(texture(TargetPos, tex_coord).xyz);
    vec3 randomVec = normalize(texture(texNoise, tex_coord*noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < MAX_KERNEL_SIZE; i++) {
        vec3 sample = TBN * Kernel[i];
        sample = fragPos + sample * radius;

        vec4 offset = vec4(sample, 1.0);
        offset = projectionMatrix * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        float sampleDepth = texture(TargetPos, offset.xy).z;

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - occlusion/MAX_KERNEL_SIZE;
    //occlusion = pow(occlusion, 2.0);

    FragColor = occlusion;
}