#type vertex
#version 430 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec3 a_Binormal;
layout (location = 4) in vec2 a_TexCoord;

uniform mat4 r_VP;
uniform mat4 r_Transform;

out VertexOutput {
    vec3 WorldPos;
    vec3 Normal;
    vec2 TexCoord;
    mat3 WorldNormals;
    mat3 WorldTransform;
    vec3 Binormal;
} vs_Output;

void main() {
    vs_Output.WorldPos = vec3(r_Transform * vec4(a_Position, 1.0));
    vs_Output.Normal = mat3(r_Transform) * a_Normal;
    vs_Output.TexCoord = vec2(a_TexCoord.x, 1.0 - a_TexCoord.y);
    vs_Output.WorldNormals = mat3(r_Transform) * mat3(a_Tangent, a_Binormal, a_Normal);
    vs_Output.WorldTransform = mat3(r_Transform);
    vs_Output.Binormal = a_Binormal;

    gl_Position = r_VP * r_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 430 core

layout (location = 0) out vec4 FragColor;

// from vertex shader
in VertexOutput {
    vec3 WorldPos;
    vec3 Normal;
    vec2 TexCoord;
    mat3 WorldNormals;
    mat3 WorldTransform;
    vec3 Binormal;
} vs_Input;

const vec3 FD = vec3(0.04);
const float PI = 3.141592;
const float Epsilon = 0.00001;

struct Light {
    vec3 Position;
    vec3 Color;
    float Strength;
};

const int MAX_LIGHTS = 2;
uniform Light r_lights[MAX_LIGHTS];
uniform vec3 r_CamPos;

// PBR Textures
uniform sampler2D u_AlbedoTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_MetalnessTexture;
uniform sampler2D u_RoughnessTexture;

// Material properties
uniform vec3 u_AlbedoColor;
uniform float u_Metalness;
uniform float u_Roughness;

// Toggles
uniform float r_AlbedoTexToggle;
uniform float r_NormalTexToggle;
uniform float r_MetalnessTexToggle;
uniform float r_RoughnessTexToggle;

// For use inside the shader
struct PBRParameters {
    vec3 Albedo;
    float Roughness;
    float Metalness;

    vec3 Normal;
    vec3 View;
    float NdotV;
};
PBRParameters m_Params;

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

// ---------------------------------------------------------------------------------------------------

vec3 Lighting(vec3 F0)
{
	vec3 result = vec3(0.0);
	for(int i = 0; i < MAX_LIGHTS; i++)
	{
		//vec3 Li = -r_lights.Direction;
		vec3 L = normalize(r_lights[i].Position - vs_Input.WorldPos);
		vec3 H = normalize(L + m_Params.View);

		float distance = length(r_lights[i].Position - vs_Input.WorldPos);
		float attenuation = 1 / (distance * distance);
		vec3 Lradiance = r_lights[i].Color * r_lights[i].Strength * attenuation;

		// Calculate angles between surface normal and various light vectors.
		float NdotL = max(0.0, dot(m_Params.Normal, L));
		float NdotH = max(0.0, dot(m_Params.Normal, H));

		float D = ndfGGX(NdotH, m_Params.Roughness);
		float G = gaSchlickGGX(NdotL, m_Params.NdotV, m_Params.Roughness);
		vec3 F = fresnelSchlickRoughness(F0, max(0.0, dot(H, m_Params.View)), m_Params.Roughness);

		vec3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
		vec3 diffuseBRDF = kd * m_Params.Albedo / PI;

		// Cook-Torrance
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * NdotL * m_Params.NdotV);

		result += (diffuseBRDF + specularBRDF) * Lradiance * NdotL;
	}
	return result;
}

void main()
{
	// Standard PBR inputs
	m_Params.Albedo = r_AlbedoTexToggle > 0.5 ? texture(u_AlbedoTexture, vs_Input.TexCoord).rgb : u_AlbedoColor; 
	m_Params.Metalness = r_MetalnessTexToggle > 0.5 ? texture(u_MetalnessTexture, vs_Input.TexCoord).r : u_Metalness;
	m_Params.Roughness = r_RoughnessTexToggle > 0.5 ?  texture(u_RoughnessTexture, vs_Input.TexCoord).r : u_Roughness;
    m_Params.Roughness = max(m_Params.Roughness, 0.05); // Minimum roughness of 0.05 to keep specular highlight

	// Normals (either from vertex or map)
	m_Params.Normal = normalize(vs_Input.Normal);
	if (r_NormalTexToggle > 0.5)
	{
		m_Params.Normal = normalize(2.0 * texture(u_NormalTexture, vs_Input.TexCoord).rgb - 1.0);
		m_Params.Normal = normalize(vs_Input.WorldNormals * m_Params.Normal);
	}

	m_Params.View = normalize(r_CamPos - vs_Input.WorldPos);
	m_Params.NdotV = max(dot(m_Params.Normal, m_Params.View), 0.0);
		
	// Specular reflection vector
	vec3 R = 2.0 * m_Params.NdotV * m_Params.Normal - m_Params.View;

	// Fresnel reflectance, metals use albedo
	vec3 F0 = mix(FD, m_Params.Albedo, m_Params.Metalness);

	vec3 lightContribution = Lighting(F0);
	vec3 iblContribution = vec3(0);//IBL(F0, R);

	FragColor = vec4(lightContribution + iblContribution, 1.0);
	//FragColor = vec4(vec3(dot(m_Params.Normal, vec3(0,1,0))), 1.0);
	//FragColor = vec4(vs_Input.WorldPos/2, 1.0);
	FragColor = vec4(lightContribution + iblContribution, 1.0) * 0.0001 + vec4(m_Params.Albedo, 1.0);
}