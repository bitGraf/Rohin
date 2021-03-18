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

uniform sampler2D r_tex1;
uniform sampler2D r_tex2;
uniform float r_mixValue;

void main() {
    vec4 sample1 = texture(r_tex1, texcoord);
    vec4 sample2 = texture(r_tex2, texcoord);

    //FragColor = mix(sample1, sample2, r_mixValue);
    FragColor = sample1 + sample2;
}