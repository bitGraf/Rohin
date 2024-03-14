#type vertex
#version 430 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec3 a_Binormal;
layout (location = 4) in vec2 a_TexCoord;

// can combine these two into ModelView matrix
layout (location = 1) uniform mat4 r_Transform;
layout (location = 2) uniform mat4 r_View;

layout (location = 3) uniform mat4 r_Projection;

out VertexOutput { // all in view-space
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
    mat3 ViewNormalMatrix;
} vs_Output;

void main() {
    mat4 model2view = r_View * r_Transform;
    mat4 normalMatrix = transpose(inverse(model2view));
    vs_Output.Position = vec3(model2view * vec4(a_Position, 1.0));
    vs_Output.Normal = vec3(normalMatrix * vec4(a_Normal, 0));
    vs_Output.TexCoord = vec2(a_TexCoord.x, 1.0 - a_TexCoord.y);
    vs_Output.ViewNormalMatrix = mat3(normalMatrix) * mat3(a_Tangent, a_Binormal, a_Normal);

    gl_Position = r_Projection * model2view * vec4(a_Position, 1.0);
}

#type fragment
#version 430 core

layout (location = 0) out vec4 out_Albedo;   //RGBA8
layout (location = 1) out vec4 out_Normal;   //RGBA16F
layout (location = 2) out vec4 out_AMR;      //RGBA8
layout (location = 3) out vec4 out_Emissive; //RGBA8
layout (location = 4) out vec4 out_Depth;    //RGBA32F

// from vertex shader
in VertexOutput {
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
    mat3 ViewNormalMatrix;
} vs_Input;

// PBR Textures
layout (location = 4) uniform sampler2D u_AlbedoTexture;
layout (location = 5) uniform sampler2D u_NormalTexture;
layout (location = 6) uniform sampler2D u_MetalnessTexture;
layout (location = 7) uniform sampler2D u_RoughnessTexture;
layout (location = 8) uniform sampler2D u_AmbientTexture;
layout (location = 9) uniform sampler2D u_EmissiveTexture;

// Material properties
layout (location = 10) uniform vec3 u_AlbedoColor;
layout (location = 11) uniform float u_Metalness;
layout (location = 12) uniform float u_Roughness;
layout (location = 13) uniform float u_TextureScale;
layout (location = 14) uniform vec3 u_EmissiveColor;

// Toggles
layout (location = 15) uniform float r_AlbedoTexToggle;
layout (location = 16) uniform float r_NormalTexToggle;
layout (location = 17) uniform float r_MetalnessTexToggle;
layout (location = 18) uniform float r_RoughnessTexToggle;
layout (location = 19) uniform float r_AmbientTexToggle;
layout (location = 20) uniform float r_EmissiveTexToggle;
layout (location = 21) uniform float r_gammaCorrect;

float linearize_depth(float d,float zNear,float zFar)
{
    float z_n = 2.0 * d - 1.0;
    return 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
}

const float gamma = 2.2;
vec3 gammaCorrect(vec3 srgb) {
    if (r_gammaCorrect > 0.5)
        return pow(srgb, vec3(gamma));
    else
        return srgb;
}

void main()
{
	// Standard PBR inputs
    vec3 Albedo = gammaCorrect(r_AlbedoTexToggle > 0.5 ? texture(u_AlbedoTexture, vs_Input.TexCoord * u_TextureScale).rgb*u_AlbedoColor : u_AlbedoColor);
    vec3 Emissive = gammaCorrect(r_EmissiveTexToggle > 0.5 ? texture(u_EmissiveTexture, vs_Input.TexCoord).rgb : u_EmissiveColor);
	float Metalness = r_MetalnessTexToggle > 0.5 ? texture(u_MetalnessTexture, vs_Input.TexCoord).g : u_Metalness;
	float Roughness = r_RoughnessTexToggle > 0.5 ?  texture(u_RoughnessTexture, vs_Input.TexCoord).b : u_Roughness;
    float Ambient = r_AmbientTexToggle > 0.5 ? texture(u_AmbientTexture, vs_Input.TexCoord).r : 1;
    Roughness = max(Roughness, 0.05); // Minimum roughness of 0.05 to keep specular highlight

	// Normals (either from vertex or map)
	vec3 Normal = normalize(vs_Input.Normal);
	if (r_NormalTexToggle > 0.5)
	{
		Normal = normalize(2.0 * texture(u_NormalTexture, vs_Input.TexCoord).rgb - 1.0);
		Normal = normalize(vs_Input.ViewNormalMatrix * Normal);
	}

    float farClipDistance = 100.0f;

    // write to render targets
    out_Albedo = vec4(Albedo, 1);
	out_Normal = vec4(Normal, 1);
    out_AMR = vec4(Ambient, Metalness, Roughness, 1);
    //out_Depth = vec4(length(vs_Input.Position.xyz), 0, 0, 1);
    out_Depth = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
    out_Emissive = vec4(Emissive, 1);
}