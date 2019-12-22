#version 330 core
out vec4 FragColor;

in vec2 tex_coord;

uniform sampler2D Target0;
uniform sampler2D Target1;
uniform sampler2D Target2;
uniform sampler2D TargetDepth;

const float gamma = 2.2;

uniform int shaderOutput;

float near = 0.01;
float far = 100.0;

float LinearizeDepth(float depth) {
	float z = depth * 2.0 - 1.0;
	return (2.0 * far * near) / (far + near - z * (far - near));
}

void main() {
    vec4 Target0_tex = texture(Target0, tex_coord);
    vec4 Target1_tex = texture(Target1, tex_coord);
    vec4 Target2_tex = texture(Target2, tex_coord);
	vec4 TargetDepth_tex = texture(TargetDepth, tex_coord);

    /* 
    Decode render targets
                R       G       B       A
    Target 0:   [     Albedo    ][ metallic ] LDR
    Target 1:   [     Normal    ][ roughness] LDR
    Target 2:   [    Emission   ][    ao    ] HDR(ao is LDR)
    */
    vec3 albedo = Target0_tex.rgb;
    vec3 normal = Target1_tex.rgb;
    vec3 emission = Target2_tex.rgb;
    float metallic = Target0_tex.a;
    float roughness = Target1_tex.a;
    float ao = Target2_tex.a;
	float depth = TargetDepth_tex.r;

    switch(shaderOutput) {
        case 0:
            FragColor = vec4(albedo, 1);
            break;
        case 1:
            FragColor = vec4(normal, 1);
            break;
        case 2:
            FragColor = vec4(albedo, 1);
            break;
        default:
            FragColor = vec4(vec3(LinearizeDepth(depth)/far), 1);
            break;
    }
}