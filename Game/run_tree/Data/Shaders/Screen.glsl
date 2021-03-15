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
out vec4 FragColor;

in vec2 texcoord;

uniform sampler2D u_tex1;
uniform sampler2D u_tex2;
uniform sampler2D u_tex3;
//uniform sampler2D u_tex4;

void main() {
    vec4 rt1 = texture(u_tex1, texcoord);
    vec4 rt2 = texture(u_tex2, texcoord);
    vec4 rt3 = texture(u_tex3, texcoord);

    vec3 Albedo = rt1.rgb;
	vec3 Normal = rt2.rgb;
    float Ambient = rt3.r;
    float Metalness = rt3.g;
    float Roughness = rt3.b;

    // this just stops the compiler from optimizing these vars 
    // out and complaining it can't find the uniforms :(
    float value = length(Albedo) + length(Normal) + Ambient + Metalness + Roughness;
    value = min(max(value, 0), 1) * 0.000001;

    FragColor = vec4(Albedo, 1+value);
}