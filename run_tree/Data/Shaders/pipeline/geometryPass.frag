#version 330 core
layout (location = 0) out vec4 Target0;
layout (location = 1) out vec4 Target1;
layout (location = 2) out vec4 Target2;

in vec3 pass_normal;
in vec3 pass_fragPos;
in vec2 pass_tex;
in mat3 pass_TBN;

struct PBRMaterial {
    sampler2D baseColorTexture;
    sampler2D normalTexture;
    sampler2D occlusionTexture;
    sampler2D metallicRoughnessTexture;
    sampler2D emissiveTexture;

    vec3 emissiveFactor;
    vec4 baseColorFactor;
    float metallicFactor;
    float roughnessFactor;
};

uniform PBRMaterial material;

void main()
{
    //Read all texture information
    vec3 albedo     = pow(vec3(texture(material.baseColorTexture, pass_tex) * material.baseColorFactor), vec3(2.2));
    vec3 normal     = pass_TBN * vec3(texture(material.normalTexture, pass_tex));
    float ao        = vec3(texture(material.occlusionTexture, pass_tex)).r;
    float roughness = material.roughnessFactor * texture(material.metallicRoughnessTexture, pass_tex).g;
    float metallic  = material.metallicFactor * texture(material.metallicRoughnessTexture, pass_tex).b;
    vec3 emission   = material.emissiveFactor * texture(material.emissiveTexture, pass_tex).rgb;

    /* 
    Assign to render targets
                R       G       B       A
    Target 0:   [     Albedo    ][ metallic ] LDR
    Target 1:   [     Normal    ][ roughness] LDR
    Target 2:   [    Emission   ][    ao    ] HDR(ao is LDR)
    */

    Target0 = vec4(albedo, metallic); 
    Target1 = vec4(normal, roughness);
    Target2 = vec4(emission, ao);
}