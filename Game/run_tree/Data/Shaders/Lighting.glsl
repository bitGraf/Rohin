#type vertex
#version 430 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

out vec2 texcoord;
out vec3 viewRay;

uniform mat4 r_Projection;

void main() {
    vec4 position = vec4(a_Position.xy, 1.0, 1.0);
	gl_Position = position;

	texcoord = a_TexCoord;
    viewRay = vec3(inverse(r_Projection) * position);
}

#type fragment
#version 430 core
layout (location = 0) out vec4 out_Diffuse;
layout (location = 1) out vec4 out_Specular;

in vec2 texcoord;
in vec3 viewRay;

struct Light {
	vec3 Position;
	vec3 Direction;
	vec3 Color;
	float Strength;
	float Inner;
	float Outer;
};

const int MAX_LIGHTS = 32;
uniform Light r_pointLights[MAX_LIGHTS];
uniform Light r_spotLights[MAX_LIGHTS];
uniform Light r_sun;

uniform sampler2D u_normal;

vec3 CalcSun(vec3 F0);
vec3 CalcPointLight(vec3 F0, Light pl);
vec3 CalcSpotLight(vec3 F0, Light sl);
vec3 Lighting(vec3 F0);

void main() {
    vec3 N = texture(u_normal, texcoord).rgb;
    vec3 vr = normalize(viewRay);
    vec3 FragPos = vec3(0,0,0);
    vec3 V = normalize(-FragPos);
    
    vec3 Color = r_sun.Color;
    vec3 LightDir = r_sun.Direction;

    out_Diffuse = vec4(Color * dot(N, -LightDir), 1);
    out_Specular = vec4(1, 1, 1, 1);
}

/*
vec3 CalcSun(vec3 F0) {
	vec3 L = -r_sun.Direction;
	vec3 H = normalize(L + m_Params.View);

	vec3 Lradiance = r_sun.Color * r_sun.Strength;

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

	return (diffuseBRDF + specularBRDF) * Lradiance * NdotL;
}

vec3 CalcPointLight(vec3 F0, Light pl) {
	vec3 L = normalize(pl.Position - vs_Input.WorldPos);
	vec3 H = normalize(L + m_Params.View);

	float distance = length(pl.Position - vs_Input.WorldPos);
	float attenuation = 1 / (distance * distance);
	vec3 Lradiance = pl.Color * pl.Strength * attenuation;

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

	return (diffuseBRDF + specularBRDF) * Lradiance * NdotL;
}

vec3 CalcSpotLight(vec3 F0, Light sl) {
	vec3 L = normalize(sl.Position - vs_Input.WorldPos);
	vec3 H = normalize(L + m_Params.View);

	float distance = length(sl.Position - vs_Input.WorldPos);
	float attenuation = 1 / (distance * distance);

	float theta     = dot(L, normalize(-sl.Direction));
	float epsilon   = sl.Inner - sl.Outer;
	float intensity = clamp((theta - sl.Outer) / epsilon, 0.0, 1.0);

	vec3 Lradiance = sl.Color * sl.Strength * attenuation * intensity;

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

	return (diffuseBRDF + specularBRDF) * Lradiance * NdotL;
}

vec3 Lighting(vec3 F0)
{
	vec3 result = vec3(0.0);
	// add directional light
	result += CalcSun(F0);

	// add up all point lights
	for(int i = 0; i < MAX_LIGHTS; i++) {
		result += CalcPointLight(F0, r_pointLights[i]);
	}
	
	// add up all spot lights
	for(int i = 0; i < MAX_LIGHTS; i++) {
		result += CalcSpotLight(F0, r_spotLights[i]);
	}

	return result;
}
*/