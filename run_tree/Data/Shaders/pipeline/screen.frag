#version 330 core
out vec4 FragColor;

in vec2 tex_coord;

uniform sampler2D Target0;
uniform sampler2D Target1;
uniform sampler2D Target2;

const float gamma = 2.2;

uniform int shaderOutput;

void main() {
    vec4 Target0_tex = texture(Target0, tex_coord);
    vec4 Target1_tex = texture(Target1, tex_coord);
    vec4 Target2_tex = texture(Target2, tex_coord);

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

    switch(shaderOutput) {
        case 0:
            FragColor = vec4(albedo, 1);
            break;
        case 1:
            FragColor = vec4(normal, 1);
            break;
        case 2:
            FragColor = vec4(emission, 1);
            break;
        case 3:
            FragColor = vec4(vec3(ao), 1);
            break;
        case 4:
            FragColor = vec4(vec3(metallic), 1);
            break;
        case 5:
            FragColor = vec4(vec3(roughness), 1);
            break;
        
        case 6:
            FragColor = vec4(albedo + vec3(ao), 1);
            break;
        case 7:
            FragColor = vec4(albedo + vec3(ao) + emission, 1);
            break;
    }
}