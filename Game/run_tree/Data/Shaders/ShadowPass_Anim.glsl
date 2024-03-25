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

layout (location = 131) uniform mat4 r_LightSpace;

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

    gl_Position = r_LightSpace * r_Transform * localPosition;
}

#type fragment
#version 430 core

layout (location = 0) out vec4 out_Depth;    //RGBA32F

void main()
{
    // write to render targets
    out_Depth = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
    //out_Depth = gl_FragCoord.z;
}