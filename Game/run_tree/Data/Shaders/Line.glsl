#type vertex
#version 430 core

layout (location = 0) in vec3 a_Position;

uniform mat4 r_VP;
uniform mat4 r_Transform;

out VertexOutput {
    vec3 WorldPos;
} vs_Output;

void main() {
	vs_Output.WorldPos = vec3(r_Transform * vec4(a_Position, 1.0));
    gl_Position = r_VP * r_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 430 core

layout (location = 0) out vec4 FragColor;

in VertexOutput {
    vec3 WorldPos;
} vs_Input;

uniform vec3 r_CamPos;
uniform vec3 r_LineColor;
uniform float r_LineFadeStart;
uniform float r_LineFadeEnd;

void main()
{
	float start = r_LineFadeStart, end = r_LineFadeEnd;
	float distance = length(r_CamPos - vs_Input.WorldPos);
	float strength = min(max((end-distance)/(end-start), 0), .75f);
	FragColor = vec4(r_LineColor, 1.0) * strength;
}