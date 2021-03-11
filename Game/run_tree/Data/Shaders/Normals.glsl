#type vertex
#version 430 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
//layout (location = 3) in vec3 a_Binormal;
//layout (location = 4) in vec2 a_TexCoord;

uniform mat4 r_Transform;

out vec3 vs_normal; //world space

void main() {
    mat3 normalMatrix = mat3(transpose(inverse(r_Transform)));
    vs_normal = normalize(vec3(vec4(normalMatrix * a_Normal, 0.0))); // world space
    //vs_normal = normalize(vec3(vec4(normalMatrix * a_Tangent, 0.0))); // world space
    //vs_normal = normalize(vec3(vec4(normalMatrix * a_Normal, 0.0))); // world space

    //vs_normal = vec3(r_Transform * vec4(a_Normal,0));
    //vs_Output.TexCoord = vec2(a_TexCoord.x, 1.0 - a_TexCoord.y);
    //vs_Output.WorldNormals = mat3(r_Transform) * mat3(a_Tangent, a_Binormal, a_Normal);
    //vs_Output.WorldTransform = mat3(r_Transform);
    //vs_Output.Binormal = a_Binormal;

    gl_Position = r_Transform * vec4(a_Position, 1.0); // world space
}

#type geometry
#version 430 core

layout(points) in;

layout(line_strip, max_vertices = 2) out;
//layout(points, max_vertices = 2) out;

in vec3 vs_normal[];
uniform mat4 r_VP;
const float normal_scale = 0.06;

void main() {
    vec4 v0 = gl_in[0].gl_Position;
    gl_Position = r_VP * v0;
    EmitVertex();

    //vec4 v1 = v0 + vec4(vs_normal[0] * normal_scale, 0);
    vec4 v1 = v0 + vec4(vs_normal[0] * normal_scale, 0);
    gl_Position = r_VP * v1;
    EmitVertex();

    EndPrimitive();
}

#type fragment
#version 430 core

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4(1,0.85,0.1, 1.0);
}