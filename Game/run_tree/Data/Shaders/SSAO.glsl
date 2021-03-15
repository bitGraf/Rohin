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
layout (location = 0) out vec4 out_SSAO;

in vec2 texcoord;

uniform sampler2D u_amr;

const vec3 FD = vec3(0.04);
const float PI = 3.141592;
const float Epsilon = 0.00001;

void main() {
    vec4 rt = texture(u_amr, texcoord);
    float baked_ao = rt.r;

    float ssao = 1-Epsilon;
    
    out_SSAO = vec4(vec3(ssao*baked_ao), 1);
}