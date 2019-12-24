#version 330 core
layout (location = 0) out float FragColor;

in vec2 tex_coord;

uniform sampler2D TargetPos;
uniform mat4 projectionMatrix;

const int MAX_KERNEL_SIZE = 16;
uniform vec3 Kernel[MAX_KERNEL_SIZE];
const float radius = 0.5;

void main() {
    vec3 pos = texture(TargetPos, tex_coord).xyz;

    float occlusion = 0.0;

    for (int i = 0; i < MAX_KERNEL_SIZE; i++) {
        vec3 samplePos = pos + Kernel[i];
        vec4 offset = vec4(samplePos, 1.0);

        offset = projectionMatrix * offset;
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + vec2(0.5);

        float sampleDepth = texture(TargetPos, offset.xy).b;

        if (abs(pos.z - sampleDepth) < radius) {
            occlusion += step(sampleDepth, samplePos.z);
        }
    }

    occlusion = 1.0 - occlusion/MAX_KERNEL_SIZE;
    occlusion = pow(occlusion, 2.0);

    FragColor = occlusion;
}