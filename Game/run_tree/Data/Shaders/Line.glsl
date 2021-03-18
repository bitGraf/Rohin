#type vertex
#version 430 core

layout (location = 0) in vec3 a_Position;

uniform mat4 r_VP;
uniform mat4 r_Transform;

out VertexOutput {
    vec3 WorldPos;
	vec2 tex_coord;
} vs_Output;

void main() {
	vs_Output.WorldPos = vec3(r_Transform * vec4(a_Position, 1.0));
    gl_Position = r_VP * r_Transform * vec4(a_Position, 1.0);
	vec3 ndc = gl_Position.xyz / gl_Position.w;
	vs_Output.tex_coord = ndc.xy * 0.5 + 0.5;
}

#type fragment
#version 430 core

layout (location = 0) out vec4 FragColor;

in VertexOutput {
    vec3 WorldPos;
	vec2 tex_coord;
} vs_Input;

uniform vec3 r_CamPos;
uniform vec3 r_LineColor;
uniform float r_LineFadeStart;
uniform float r_LineFadeEnd;
uniform float r_LineFadeMaximum;
uniform float r_LineFadeMinimum;

//const mat4 ditherPattern = mat4(
//    vec4( 0.0f, 0.5f, 0.125f, 0.625f),
//    vec4( 0.75f, 0.22f, 0.875f, 0.375f),
//    vec4( 0.1875f, 0.6875f, 0.0625f, 0.5625),
//    vec4( 0.9375f, 0.4375f, 0.8125f, 0.3125));

void main()
{
	float start = r_LineFadeStart, end = r_LineFadeEnd;
	float distance = length(r_CamPos - vs_Input.WorldPos);
	float strength = min(max((end-distance)/(end-start), r_LineFadeMinimum), r_LineFadeMaximum);

	//int screenX = int(vs_Input.tex_coord.x * 1280 * 4.0f);
    //int screenY = int(vs_Input.tex_coord.y * 720 * 4.0f);
	//float ditherPattern = ditherPattern[screenX % 16][screenY % 16];

	FragColor = vec4(r_LineColor, 1) * strength;
}