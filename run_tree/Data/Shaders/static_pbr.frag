#version 330 core
out vec4 FragColor;

in vec3 pass_normal;
in vec3 pass_fragPos;
in vec2 pass_tex;
in mat3 pass_TBN;

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
	float cutoff;
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
uniform PointLight pointLights[NUMPOINTLIGHTS];
uniform DirectionalLight sun;
uniform SpotLight spotLight;

uniform PBRMaterial material;
uniform vec3 camPos;

//uniform samplerCube irradianceMap;
//uniform samplerCube prefilterMap;
//uniform sampler2D   brdfLUT;

const float PI = 3.14159265359;
const float outerCutoff = cos(40*PI/180);

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

void main()
{
    //Read all texture information
    vec3 albedo = vec3(texture(material.baseColorTexture, pass_tex) * material.baseColorFactor);
    vec3 normal = pass_TBN * vec3(texture(material.normalTexture, pass_tex));
    float ao = vec3(texture(material.occlusionTexture, pass_tex)).r;
    float metallic = material.metallicFactor * texture(material.metallicRoughnessTexture, pass_tex).r;
    float roughness = material.roughnessFactor * texture(material.metallicRoughnessTexture, pass_tex).g;

	normal = pass_normal;
    
    //Calcualte tangent space normal
    vec3 N = normal;
    vec3 V = normalize(camPos - pass_fragPos);
    vec3 R = reflect(-V, N);
    
    vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic);
    
    //Direct Lighting
    vec3 Lo = calcTotalLightContribution(pointLights, sun, 
		  N, pass_fragPos, V, F0, roughness, metallic, albedo);
    
    vec3 color = vec3(.025) * albedo * ao + Lo;
    
    FragColor = vec4(color, 1.0);
	//FragColor = vec4(1,0,0,1);
	//FragColor = vec4(N, 1);
    //FragColor = vec4(albedo, 1);
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

	//Point lights
    for (int i = 0; i < NUMPOINTLIGHTS; i++) {
        Lo += addPointLight(pointLights[i], normal, fragPos, viewDir, F0, roughness, metallic, albedo);
    }

	//Spot lights
	Lo += addSpotLight(spotLight, normal, fragPos, viewDir, F0, roughness, metallic, albedo);

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
	float epsilon = light.cutoff - outerCutoff;
	float intensity = clamp((theta -  outerCutoff) / epsilon, 0.0, 1.0);
        
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