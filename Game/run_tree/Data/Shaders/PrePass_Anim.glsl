#type vertex
#version 430 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec3 a_Bitangent;
layout (location = 4) in vec2 a_TexCoord;
layout (location = 5) in ivec4 a_BoneIndices;
layout (location = 6) in vec4 a_BoneWeights;

const int MAX_BONES = 128;
layout (location = 1) uniform mat4 r_Bones[MAX_BONES]; // 128 slots
layout (location = 129) uniform int r_UseSkin;

// can combine these two into ModelView matrix
layout (location = 130) uniform mat4 r_Transform;
layout (location = 131) uniform mat4 r_View;

layout (location = 132) uniform mat4 r_Projection;

out VertexOutput { // all in view-space
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
    mat3 ViewNormalMatrix;
} vs_Output;

void main() {
    float finalWeight = 1 - a_BoneWeights[0] - a_BoneWeights[1] - a_BoneWeights[2]; // ensure total weight is 1
    mat4 boneTransform = r_Bones[a_BoneIndices[0]] * a_BoneWeights[0];
    boneTransform += r_Bones[a_BoneIndices[1]] * a_BoneWeights[1];
    boneTransform += r_Bones[a_BoneIndices[2]] * a_BoneWeights[2];
    boneTransform += r_Bones[a_BoneIndices[3]] * finalWeight;

    vec4 localPosition = vec4(a_Position, 1.0);
    if (r_UseSkin==1) {
        localPosition = boneTransform * localPosition;
    }

    mat4 model2view = r_View * r_Transform;
    mat4 normalMatrix = transpose(inverse(model2view));
    vs_Output.Position = vec3(model2view * localPosition);
    vs_Output.Normal = vec3(normalMatrix * vec4(a_Normal, 0));
    vs_Output.TexCoord = vec2(a_TexCoord.x, 1.0 - a_TexCoord.y);
    vs_Output.ViewNormalMatrix = mat3(normalMatrix) * mat3(a_Tangent, a_Bitangent, a_Normal);

    gl_Position = r_Projection * model2view * localPosition;
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
layout (location = 133) uniform sampler2D u_AlbedoTexture;
layout (location = 134) uniform sampler2D u_NormalTexture;
layout (location = 135) uniform sampler2D u_MetalnessTexture;
layout (location = 136) uniform sampler2D u_RoughnessTexture;
layout (location = 137) uniform sampler2D u_AmbientTexture;
layout (location = 138) uniform sampler2D u_EmissiveTexture;

// Material properties
layout (location = 139) uniform vec3 u_AlbedoColor;
layout (location = 140) uniform float u_Metalness;
layout (location = 141) uniform float u_Roughness;
layout (location = 142) uniform float u_TextureScale;
layout (location = 143) uniform vec3 u_EmissiveColor;

// Toggles
layout (location = 144) uniform float r_AlbedoTexToggle;
layout (location = 145) uniform float r_NormalTexToggle;
layout (location = 146) uniform float r_MetalnessTexToggle;
layout (location = 147) uniform float r_RoughnessTexToggle;
layout (location = 148) uniform float r_AmbientTexToggle;
layout (location = 149) uniform float r_EmissiveTexToggle;
layout (location = 150) uniform float r_gammaCorrect;

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