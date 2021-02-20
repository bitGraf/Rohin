#type vertex
#version 430 core
layout (location = 0) in vec3 a_Position;

out vec3 frag_pos;

uniform mat4 r_inverseVP;

void main() {
    vec4 position = vec4(a_Position.xy, 1.0, 1.0);
	gl_Position = position;

	frag_pos = (inverse(r_inverseVP) * position).xyz;
}

#type fragment
#version 430 core
out vec4 FragColor;

in vec3 frag_pos;

uniform samplerCube r_skybox;

void main() {
    FragColor = texture(r_skybox, frag_pos);
}