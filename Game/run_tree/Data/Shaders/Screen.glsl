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

uniform sampler2D u_albedo;
uniform sampler2D u_normal;
uniform sampler2D u_amr;
uniform sampler2D u_depth;
uniform sampler2D u_diffuse;
uniform sampler2D u_specular;
uniform sampler2D u_emissive;

uniform int r_outputSwitch;

float makeDepthPretty(float d);

void main() {
    vec4 rt1 = texture(u_albedo, texcoord);
    vec4 rt2 = texture(u_normal, texcoord);
    vec4 rt3 = texture(u_amr, texcoord);
    vec4 rt4 = texture(u_depth, texcoord);
    vec4 rt5 = texture(u_diffuse, texcoord);
    vec4 rt6 = texture(u_specular, texcoord);
    vec4 rt7 = texture(u_emissive, texcoord);

    vec3 Albedo = rt1.rgb;
	vec3 Normal = rt2.rgb;
    float Ambient = rt3.r;
    float Metalness = rt3.g;
    float Roughness = rt3.b;
    float depth = rt4.r;
    vec3 Diffuse = rt5.rgb;
    vec3 Specular = rt6.rgb;
    vec3 Emission = rt7.rgb;

    // this just stops the compiler from optimizing these vars 
    // out and complaining it can't find the uniforms :(
    //float value = length(Albedo) + length(Normal) + Ambient + Metalness + Roughness + length(Diffuse) + length(Specular);
    //value = min(max(value, 0), 1) * 0.000001;

    if (r_outputSwitch == 0)
        FragColor = vec4(Albedo, 1);
    else if (r_outputSwitch == 1)
        FragColor = vec4(Normal, 1);
    else if (r_outputSwitch == 2)
        FragColor = vec4(Ambient, Ambient, Ambient, 1);
    else if (r_outputSwitch == 3)
        FragColor = vec4(Metalness, Metalness, Metalness, 1);
    else if (r_outputSwitch == 4)
        FragColor = vec4(Roughness, Roughness, Roughness, 1);
    else if (r_outputSwitch == 5)
        FragColor = vec4(rt3.rgb, 1);
    else if (r_outputSwitch == 6)
        FragColor = vec4(vec3(makeDepthPretty(depth)), 1);
    else if (r_outputSwitch == 7)
        FragColor = vec4(Diffuse, 1);
    else if (r_outputSwitch == 8)
        FragColor = vec4(Specular, 1);
    else if (r_outputSwitch == 9)
        FragColor = vec4(Emission, 1);
    else if (r_outputSwitch == 10)
        FragColor = vec4((Albedo*Diffuse + Specular)*Ambient + Emission, 1);
    else
        FragColor = vec4(1, 0, 0, 1);
}

float makeDepthPretty(float d) {
    return d/15;
}