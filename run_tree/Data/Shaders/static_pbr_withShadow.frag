#version 330 core
out vec4 FragColor;

in vec3 pass_normal;
in vec3 pass_fragPos;
in vec2 pass_tex;
in mat3 pass_TBN;
in vec3 pass_tangent;
in vec4 pass_fragPosLightSpace;

struct PointLight {
    vec3 position;
    vec4 color;
	float strength;
};

struct DirectionalLight {
	vec3 direction;
	vec4 color;
	float strength;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	vec4 color;
	float strength;
	float inner_cutoff;
	float outer_cutoff;
};

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

const int NUMPOINTLIGHTS = 4;
const int NUMSPOTLIGHTS = 4;

uniform DirectionalLight sun;
uniform PointLight pointLights[NUMPOINTLIGHTS];
uniform SpotLight spotLights[NUMSPOTLIGHTS];

uniform PBRMaterial material;
uniform vec3 camPos;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;
uniform sampler2D   shadowMap;

const float PI = 3.14159265359;
const int ShadowSamples = 3; //NEEDS TO BE ODD

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0, float roughness);

vec3 calcTotalLightContribution(PointLight pointLights[NUMPOINTLIGHTS], DirectionalLight dirLight, 
								vec3 normal, vec3 fragPos, 
							    vec3 viewDir, vec3 F0, float roughness, 
							    float metallic, vec3 albedo);
vec3 addPointLight(PointLight light, vec3 normal, vec3 fragPos, 
				   vec3 viewDir, vec3 F0, float roughness, 
				   float metallic, vec3 albedo);
vec3 addDirLight(DirectionalLight light, vec3 normal, vec3 fragPos, 
				   vec3 viewDir, vec3 F0, float roughness, 
				   float metallic, vec3 albedo);
vec3 addSpotLight(SpotLight light, vec3 normal, vec3 fragPos, 
				   vec3 viewDir, vec3 F0, float roughness, 
				   float metallic, vec3 albedo);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

void main()
{
    //Read all texture information
    vec3 albedo = vec3(texture(material.baseColorTexture, pass_tex) * material.baseColorFactor);
    vec3 normal2 = pass_TBN * vec3(texture(material.normalTexture, pass_tex));
    float ao = vec3(texture(material.occlusionTexture, pass_tex)).r;
    float roughness = material.roughnessFactor * texture(material.metallicRoughnessTexture, pass_tex).g;
    float metallic = material.metallicFactor * texture(material.metallicRoughnessTexture, pass_tex).b;
    vec3 emission = material.emissiveFactor * texture(material.emissiveTexture, pass_tex).rgb;

    normal2 = normalize(normal2 * 2.0 - 1.0);

    vec3 normal = pass_normal;
    
    //Calcualte tangent space normal
    vec3 N = normal;
    vec3 V = normalize(camPos - pass_fragPos);
    vec3 R = reflect(-V, N);
    
    vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic);
    
    //Direct Lighting
    vec3 Lo = calcTotalLightContribution(pointLights, sun, 
		  N, pass_fragPos, V, F0, roughness, metallic, albedo);

    vec3 F = fresnelSchlick(max(dot(N, V), 0.0), F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse    = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness*MAX_REFLECTION_LOD).rgb;
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F*brdf.x + brdf.y);

    vec3 ambient    = (kD * diffuse + specular) * ao;
    
    vec3 color = ambient + Lo + emission;
    
    FragColor = vec4(color, 1);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / max(denom, 0.001);
}
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    
    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 calcTotalLightContribution(PointLight pointLights[NUMPOINTLIGHTS], DirectionalLight dirLight, 
								vec3 normal, vec3 fragPos, 
							    vec3 viewDir, vec3 F0, float roughness, 
							    float metallic, vec3 albedo) {
	//Direct Lighting
    vec3 Lo = vec3(0.0);

	//Direction Lights
    Lo += addDirLight(dirLight, normal, fragPos, viewDir, F0, roughness, metallic, albedo);
    Lo *= (1 - ShadowCalculation(pass_fragPosLightSpace, normal, dirLight.direction));

	//Point lights
    for (int i = 0; i < NUMPOINTLIGHTS; i++) {
        //Lo += addPointLight(pointLights[i], normal, fragPos, viewDir, F0, roughness, metallic, albedo);
    }

	//Spot lights
	for (int i = 0; i < NUMSPOTLIGHTS; i++) {
		//Lo += addSpotLight(spotLights[i], normal, fragPos, viewDir, F0, roughness, metallic, albedo);
    }

	return Lo;
}

vec3 addDirLight(DirectionalLight light, vec3 normal, vec3 fragPos, 
				   vec3 viewDir, vec3 F0, float roughness, 
				   float metallic, vec3 albedo) {
	vec3 L = normalize(-light.direction);
    vec3 H = normalize(viewDir + L);
        
    vec3 radiance = light.color.xyz * light.strength;
        
    float NDF = DistributionGGX(normal, H, roughness);
    float G = GeometrySmith(normal, viewDir, L, roughness);
    vec3 F = fresnelSchlick(clamp(dot(H, viewDir), 0.0, 1.0), F0, roughness);
        
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0) * max(dot(normal, L), 0);
    vec3 specular = numerator / max(denominator, 0.001);
        
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
        
    float NdotL = max(dot(normal, L), 0);
    vec3 Lo = (kD*albedo / PI + specular) * radiance * NdotL;

	return Lo;
}

vec3 addPointLight(PointLight light, vec3 normal, vec3 fragPos, 
				   vec3 viewDir, vec3 F0, float roughness, 
				   float metallic, vec3 albedo) {
	vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(viewDir + L);
        
    float distance = length(light.position - pass_fragPos);
    float attenuation = 1 / (distance * distance);
    vec3 radiance = light.color.xyz * attenuation * light.strength;
        
    float NDF = DistributionGGX(normal, H, roughness);
    float G = GeometrySmith(normal, viewDir, L, roughness);
    vec3 F = fresnelSchlick(clamp(dot(H, viewDir), 0.0, 1.0), F0, roughness);
        
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0) * max(dot(normal, L), 0);
    vec3 specular = numerator / max(denominator, 0.001);
        
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
        
    float NdotL = max(dot(normal, L), 0);
    vec3 Lo = (kD*albedo / PI + specular) * radiance * NdotL;

	return Lo;
}

vec3 addSpotLight(SpotLight light, vec3 normal, vec3 fragPos, 
				   vec3 viewDir, vec3 F0, float roughness, 
				   float metallic, vec3 albedo) {
	vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(viewDir + L);

	float theta = dot(L, normalize(-light.direction));
	float epsilon = light.inner_cutoff - light.outer_cutoff;
	float intensity = clamp((theta -  light.outer_cutoff) / epsilon, 0.0, 1.0);
        
    float distance = length(light.position - pass_fragPos);
    float attenuation = 1 / (distance * distance);
    vec3 radiance = light.color.xyz * attenuation * light.strength * intensity;
        
    float NDF = DistributionGGX(normal, H, roughness);
    float G = GeometrySmith(normal, viewDir, L, roughness);
    vec3 F = fresnelSchlick(clamp(dot(H, viewDir), 0.0, 1.0), F0, roughness);
        
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0) * max(dot(normal, L), 0);
    vec3 specular = numerator / max(denominator, 0.001);
        
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
        
    float NdotL = max(dot(normal, L), 0);
    vec3 Lo = (kD*albedo / PI + specular) * radiance * NdotL;

	return Lo;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -(ShadowSamples/2); x <= ShadowSamples/2; ++x) {
        for (int y = -(ShadowSamples/2); y <= ShadowSamples/2; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y)*texelSize).r;
            shadow += currentDepth-bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= (ShadowSamples*ShadowSamples);

    if (projCoords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}