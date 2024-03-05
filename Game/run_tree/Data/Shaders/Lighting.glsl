#type vertex
#version 430 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

out vec2 texcoord;

void main() {
    vec4 position = vec4(a_Position.xy, 1.0, 1.0);
	gl_Position = position;

	texcoord = a_TexCoord;
}

#type fragment
#version 430 core
layout (location = 0) out vec4 out_Diffuse;
layout (location = 1) out vec4 out_Specular;

in vec2 texcoord;

// Light => 6 uniform slots each
struct Light {
	vec3 Position;
	vec3 Direction;
	vec3 Color;
	float Strength;
	float Inner;
	float Outer;
};

layout (location = 1) uniform mat4 r_Projection;

const int MAX_LIGHTS = 32;
layout (location = 2) uniform Light r_pointLights[MAX_LIGHTS]; // 192 slots
layout (location = 194) uniform Light r_spotLights[MAX_LIGHTS]; // 192 slots
layout (location = 386) uniform Light r_sun; // 6 slots

layout (location = 392) uniform mat4 r_View;

layout (location = 393) uniform sampler2D u_albedo;
layout (location = 394) uniform sampler2D u_normal;
layout (location = 395) uniform sampler2D u_depth;
layout (location = 396) uniform sampler2D u_amr;

// PBR IBL cubemaps
layout (location = 397) uniform samplerCube u_irradiance;
layout (location = 398) uniform samplerCube u_prefilter;
layout (location = 399) uniform sampler2D   u_brdf_LUT;

const vec3 FD = vec3(0.04);
const float PI = 3.141592;
const float Epsilon = 0.00001;

// For use inside the shader
struct PBRParameters {
	vec3 Albedo;

    float Roughness;
    float Metalness;
	float AO;

    vec3 Normal;
	vec3 NormalWorld;
    vec3 View;
    vec3 FragPos;
    float NdotV;

    vec3 Diffuse;
    vec3 Specular;
};
PBRParameters m_Params;

void Lighting(vec3 F0);
void IBL(vec3 F0, vec3 R);
vec3 FragPosFromDepth(float depth);

void main() {
	m_Params.Albedo = texture(u_albedo, texcoord).rgb;
    vec4 amr_ = texture(u_amr, texcoord);
    m_Params.Metalness = amr_.g;
    m_Params.Roughness = max(amr_.b, 0.05);
	m_Params.AO = amr_.r;
    m_Params.Normal = normalize(texture(u_normal, texcoord).rgb);
	m_Params.NormalWorld = normalize((inverse(r_View)*vec4(m_Params.Normal, 0)).xyz);
    float depth = texture(u_depth, texcoord).r;
	m_Params.FragPos = FragPosFromDepth(depth);
    m_Params.View = normalize(-m_Params.FragPos);
    m_Params.NdotV = max(dot(m_Params.Normal, m_Params.View), 0.0);
    
    // Specular reflection vector
	vec3 R = 2.0 * m_Params.NdotV * m_Params.Normal - m_Params.View;
	vec3 RWorld = normalize((inverse(r_View)*vec4(R, 0)).xyz);

    // Fresnel reflectance, metals use albedo
	vec3 F0 = mix(FD, m_Params.Albedo, m_Params.Metalness);

    Lighting(F0);
    IBL(F0, RWorld);

    out_Diffuse = vec4(m_Params.Diffuse, 1);
    out_Specular = vec4(m_Params.Specular, 1);
}

// reconstruct view-space frag position from depth buffer
vec3 FragPosFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(texcoord * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverse(r_Projection) * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2
float ndfGGX(float cosLh, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(NdotV, k);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(vec3 F0, float cosTheta, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
} 

void CalcSunDirect(vec3 F0) {
    vec3 sun_dir = vec3(r_View * vec4(r_sun.Direction, 0));

	vec3 L = normalize(-sun_dir);
	vec3 H = normalize(L + m_Params.View);

	vec3 Lradiance = r_sun.Color * r_sun.Strength;

	// Calculate angles between surface normal and various light vectors.
	float NdotL = max(0.0, dot(m_Params.Normal, L));
    float NdotH = max(0.0, dot(m_Params.Normal, H));

    float D = ndfGGX(NdotH, m_Params.Roughness);
	float G = gaSchlickGGX(NdotL, m_Params.NdotV, m_Params.Roughness);
	vec3 F = fresnelSchlick(F0, max(0.0, dot(H, m_Params.View)));

	vec3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
	vec3 diffuseBRDF = kd * m_Params.Albedo / PI;

    // Cook-Torrance
	vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * NdotL * m_Params.NdotV);

    m_Params.Diffuse += diffuseBRDF * Lradiance * NdotL;
    m_Params.Specular += specularBRDF * Lradiance * NdotL;
}

void CalcPointLightDirect(vec3 F0, Light pl) {
    pl.Position = vec3(r_View * vec4(pl.Position, 1));

	vec3 L = normalize(pl.Position - m_Params.FragPos);
	vec3 H = normalize(L + m_Params.View);

	float distance = max(length(pl.Position - m_Params.FragPos), .01);
	float attenuation = min(1 / (distance * distance), 1);
	vec3 Lradiance = pl.Color * pl.Strength * attenuation;

	// Calculate angles between surface normal and various light vectors.
	float NdotL = max(0.0, dot(m_Params.Normal, L));
    float NdotH = max(0.0, dot(m_Params.Normal, H));

    float D = ndfGGX(NdotH, m_Params.Roughness);
	float G = gaSchlickGGX(NdotL, m_Params.NdotV, m_Params.Roughness);
    vec3 F = fresnelSchlick(F0, max(0.0, dot(H, m_Params.View)));

	vec3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
	vec3 diffuseBRDF = kd * m_Params.Albedo / PI;

    // Cook-Torrance
	vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * NdotL * m_Params.NdotV);

	m_Params.Diffuse += diffuseBRDF * Lradiance * NdotL;
    m_Params.Specular += specularBRDF * Lradiance * NdotL;
}

void CalcSpotLightDirect(vec3 F0, Light sl) {
    sl.Position  = vec3(r_View * vec4(sl.Position, 1));
    sl.Direction = vec3(r_View * vec4(sl.Direction, 0));

	vec3 L = normalize(sl.Position - m_Params.FragPos);
	vec3 H = normalize(L + m_Params.View);

	float distance = max(length(sl.Position - m_Params.FragPos), .01);
	float attenuation = min(1 / (distance * distance), 1);

	float theta     = dot(L, normalize(-sl.Direction));
	float epsilon   = sl.Inner - sl.Outer;
	float intensity = clamp((theta - sl.Outer) / epsilon, 0.0, 1.0);

	vec3 Lradiance = sl.Color * sl.Strength * attenuation * intensity;

	// Calculate angles between surface normal and various light vectors.
	float NdotL = max(0.0, dot(m_Params.Normal, L));
    float NdotH = max(0.0, dot(m_Params.Normal, H));

    float D = ndfGGX(NdotH, m_Params.Roughness);
	float G = gaSchlickGGX(NdotL, m_Params.NdotV, m_Params.Roughness);
	vec3 F = fresnelSchlick(F0, max(0.0, dot(H, m_Params.View)));

	vec3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
	vec3 diffuseBRDF = kd * m_Params.Albedo / PI;

    // Cook-Torrance
	vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * NdotL * m_Params.NdotV);

	m_Params.Diffuse += diffuseBRDF * Lradiance * NdotL;
    m_Params.Specular += specularBRDF * Lradiance * NdotL;
}

void Lighting(vec3 F0) {
	m_Params.Diffuse  = vec3(0.0);
    m_Params.Specular = vec3(0.0);

	// add directional light
	CalcSunDirect(F0);

	// add up all point lights
	for(int i = 0; i < MAX_LIGHTS; i++) {
		CalcPointLightDirect(F0, r_pointLights[i]);
	}
	
	// add up all spot lights
	for(int i = 0; i < MAX_LIGHTS; i++) {
		CalcSpotLightDirect(F0, r_spotLights[i]);
	}
}

void IBL(vec3 F0, vec3 R) {
	// Diffuse Irradiance
	vec3 kS = fresnelSchlick(F0, max(m_Params.NdotV, 0.0));
	vec3 kD = 1.0 - kS;
	vec3 irradiance = texture(u_irradiance, m_Params.NormalWorld).rgb;
	vec3 diffuse    = irradiance * m_Params.Albedo;
	vec3 ambient    = (kD * diffuse); 

	// Specular IBL
	const float MAX_REFLECTION_LOD = 5.0; // since we generate mipmaps 0-5 (6 total)
    vec3 prefilteredColor = textureLod(u_prefilter, R,  m_Params.Roughness * MAX_REFLECTION_LOD).rgb;

	vec3 F        = fresnelSchlickRoughness(F0, max(m_Params.NdotV, 0.0), m_Params.Roughness);
	vec2 envBRDF  = texture(u_brdf_LUT, vec2(max(m_Params.NdotV, 0.0), m_Params.Roughness)).rg;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
    
	// Indiriect LIghting
	m_Params.Diffuse  += ambient * m_Params.AO;
	m_Params.Specular += specular * m_Params.AO;
}