#type vertex
#version 430 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec3 a_Binormal;
layout (location = 4) in vec2 a_TexCoord;
layout (location = 5) in ivec4 a_BoneIndices;
layout (location = 6) in vec4 a_BoneWeights;

const int MAX_BONES = 64;
struct Bone {
    vec3 Position;
    vec4 Orientation; //quat
};
uniform Bone r_Bones[MAX_BONES];

// can combine these two into ModelView matrix
uniform mat4 r_Transform;
uniform mat4 r_View;

uniform mat4 r_Projection;

out VertexOutput { // all in view-space
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
    mat3 ViewNormalMatrix;
} vs_Output;

vec4 qinv(vec4 q) {
    return vec4(-q.x, -q.y, -q.z, q.w);
}

vec4 qmul (vec4 q1, vec4 q2) {
    return vec4(q1.xyz*q2.w + q2.xyz*q1.w + cross(q2.xyz,q1.xyz), q1.w*q2.w - dot(q1.xyz, q2.xyz));

    //return vec4(
    //    q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
    //    q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
    //    q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
    //    q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
}

vec3 transform(Bone bone) {
    vec4 Q = bone.Orientation;
    vec4 Qp = qinv(bone.Orientation);
    vec4 V = vec4(a_Position, 0);

    // Qp * V * Q
    vec4 Vp = qmul(Qp, qmul(V, Q));
    return vec3(Vp) + bone.Position;
}

vec3 calcSkinnedVert() {
    vec3 pos = vec3(0.0);
    pos += transform(r_Bones[a_BoneIndices.x]) * a_BoneWeights.x;
    pos += transform(r_Bones[a_BoneIndices.y]) * a_BoneWeights.y;
    pos += transform(r_Bones[a_BoneIndices.z]) * a_BoneWeights.z;
    pos += transform(r_Bones[a_BoneIndices.w]) * a_BoneWeights.w;
    return pos;
}

void main() {
    vec3 skinnedPos = calcSkinnedVert();

    mat4 model2view = r_View * r_Transform;
    mat4 normalMatrix = transpose(inverse(model2view));
    vs_Output.Position = vec3(model2view * vec4(skinnedPos, 1.0));
    vs_Output.Normal = vec3(normalMatrix * vec4(a_Normal, 0));
    vs_Output.TexCoord = vec2(a_TexCoord.x, 1.0 - a_TexCoord.y);
    vs_Output.ViewNormalMatrix = mat3(normalMatrix) * mat3(a_Tangent, a_Binormal, a_Normal);

    gl_Position = r_Projection * model2view * vec4(skinnedPos, 1.0);
    //(1,2,3,4) x (5,2,-1,4)
    //gl_Position = qmul(vec4(2,3,4,1), vec4(2,-1,4,5));
}

#type fragment
#version 430 core

layout (location = 0) out vec4 out_Albedo; //RGBA8
layout (location = 1) out vec4 out_Normal; //RGBA16F
layout (location = 2) out vec4 out_AMR;    //RGBA8
layout (location = 3) out vec4 out_Depth;  //R32F
layout (location = 4) out vec4 out_Emissive;  //RGBA8

// from vertex shader
in VertexOutput {
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
    mat3 ViewNormalMatrix;
} vs_Input;

// PBR Textures
uniform sampler2D u_AlbedoTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_MetalnessTexture;
uniform sampler2D u_RoughnessTexture;
uniform sampler2D u_AmbientTexture;
uniform sampler2D u_EmissiveTexture;

// Material properties
uniform vec3 u_AlbedoColor;
uniform float u_Metalness;
uniform float u_Roughness;
uniform float u_TextureScale;

// Toggles
uniform float r_AlbedoTexToggle;
uniform float r_NormalTexToggle;
uniform float r_MetalnessTexToggle;
uniform float r_RoughnessTexToggle;
uniform float r_AmbientTexToggle;
uniform float r_EmissiveTexToggle;
uniform float r_gammaCorrect;

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
	vec3 Albedo = gammaCorrect(r_AlbedoTexToggle > 0.5 ? texture(u_AlbedoTexture, vs_Input.TexCoord * u_TextureScale).rgb : u_AlbedoColor);
    vec3 Emissive = gammaCorrect(r_EmissiveTexToggle > 0.5 ? texture(u_EmissiveTexture, vs_Input.TexCoord).rgb : vec3(0));
	float Metalness = r_MetalnessTexToggle > 0.5 ? texture(u_MetalnessTexture, vs_Input.TexCoord).r : u_Metalness;
	float Roughness = r_RoughnessTexToggle > 0.5 ?  texture(u_RoughnessTexture, vs_Input.TexCoord).r : u_Roughness;
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
    out_Depth = vec4(length(vs_Input.Position.xyz), 0, 0, 1);
    out_Emissive = vec4(Emissive, 1);
}