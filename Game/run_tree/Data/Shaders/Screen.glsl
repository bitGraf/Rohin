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
uniform sampler2D u_ssao;

uniform int r_outputSwitch;
uniform float r_toneMap;
uniform float r_gammaCorrect;

float makeDepthPretty(float d);
vec3 ToneMap(vec3 colorIn);

void main() {
    vec4 rt1 = texture(u_albedo, texcoord);
    vec4 rt2 = texture(u_normal, texcoord);
    vec4 rt3 = texture(u_amr, texcoord);
    vec4 rt4 = texture(u_depth, texcoord);
    vec4 rt5 = texture(u_diffuse, texcoord);
    vec4 rt6 = texture(u_specular, texcoord);
    vec4 rt7 = texture(u_emissive, texcoord);
    vec4 rt8 = texture(u_ssao, texcoord);

    vec3 Albedo = rt1.rgb;
	vec3 Normal = rt2.rgb;
    float Ambient = rt3.r;
    float Metalness = rt3.g;
    float Roughness = rt3.b;
    float depth = rt4.r;
    vec3 Diffuse = rt5.rgb;
    vec3 Specular = rt6.rgb;
    vec3 Emission = rt7.rgb;
    float SSAO = rt8.r;

    // this just stops the compiler from optimizing these vars 
    // out and complaining it can't find the uniforms :(
    //float value = length(Albedo) + length(Normal) + Ambient + Metalness + Roughness + length(Diffuse) + length(Specular);
    //value = min(max(value, 0), 1) * 0.000001;

    if (r_outputSwitch == 0)
        FragColor = vec4(ToneMap((Albedo*Diffuse + Specular)*Ambient) + Emission, 1);
    else if (r_outputSwitch == 1)
        FragColor = vec4(Albedo, 1);
    else if (r_outputSwitch == 2)
        FragColor = vec4(Normal, 1);
    else if (r_outputSwitch == 3)
        FragColor = vec4(Ambient, Ambient, Ambient, 1);
    else if (r_outputSwitch == 4)
        FragColor = vec4(Metalness, Metalness, Metalness, 1);
    else if (r_outputSwitch == 5)
        FragColor = vec4(Roughness, Roughness, Roughness, 1);
    else if (r_outputSwitch == 6)
        FragColor = vec4(rt3.rgb, 1);
    else if (r_outputSwitch == 7)
        FragColor = vec4(vec3(makeDepthPretty(depth)), 1);
    else if (r_outputSwitch == 8)
        FragColor = vec4(ToneMap(Diffuse), 1);
    else if (r_outputSwitch == 9)
        FragColor = vec4(ToneMap(Specular), 1);
    else if (r_outputSwitch == 10)
        FragColor = vec4(Emission, 1);
    else if (r_outputSwitch == 11)
        FragColor = vec4(vec3(SSAO), 1);
    else
        FragColor = vec4(1, 0, 0, 1);

    // Gamma correct
    float gamma = 2.2;
    if (r_gammaCorrect > 0.5)
        FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}

float makeDepthPretty(float d) {
    return d/15;
}

// from http://www.adriancourreges.com/blog/2017/12/15/mgs-v-graphics-study/
float func(float x) {
    float A = 0.6, B = 0.45333;
    if (x <= A) {
        return x;
    } else {
        return min(1, A+B-B*B/(x-A+B));
    }
}

vec3 MGSVToneMap(vec3 color) {
    return vec3(func(color.x), func(color.y), func(color.z));
}

vec3 uncharted2_tonemap_partial(vec3 x)
{
    float A = 0.15f;
    float B = 0.50f;
    float C = 0.10f;
    float D = 0.20f;
    float E = 0.02f;
    float F = 0.30f;
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 uncharted2_filmic(vec3 v)
{
    float exposure_bias = 2.0f;
    vec3 curr = uncharted2_tonemap_partial(v * exposure_bias);

    vec3 W = vec3(11.2f);
    vec3 white_scale = vec3(1.0f) / uncharted2_tonemap_partial(W);
    return curr * white_scale;
}

float luminance(vec3 v)
{
    return dot(v, vec3(0.2126f, 0.7152f, 0.0722f));
}

vec3 change_luminance(vec3 c_in, float l_out)
{
    float l_in = luminance(c_in);
    return c_in * (l_out / l_in);
}

vec3 reinhard_extended_luminance(vec3 v, float max_white_l)
{
    float l_old = luminance(v);
    float numerator = l_old * (1.0f + (l_old / (max_white_l * max_white_l)));
    float l_new = numerator / (1.0f + l_old);
    return change_luminance(v, l_new);
}

const mat3 ACESInputMat = {
    {0.59719, 0.35458, 0.04823},
    {0.07600, 0.90834, 0.01566},
    {0.02840, 0.13383, 0.83777}
};

const mat3 ACESOutputMat = {
    { 1.60475, -0.53108, -0.07367},
    {-0.10208,  1.10813, -0.00605},
    {-0.00327, -0.07276,  1.07602}
};

vec3 RRTAndODTFit(vec3 v)
{
    vec3 a = v * (v + 0.0245786f) - 0.000090537f;
    vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

// These matrices should be transposed offline
vec3 ACESFitted(vec3 color) {
    color = transpose(ACESInputMat) * color;

    // Apply RRT and ODT
    color = RRTAndODTFit(color);

    color = transpose(ACESOutputMat) * color;

    // Clamp to [0,1]
    color = clamp(color, 0.0, 1.0);

    return color;
}

vec3 ACES_approx(vec3 color) {
    color *= 0.6f;
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((color*(a*color+b))/(color*(c*color+d)+e), 0.0f, 1.0f);
}

vec3 ToneMap(vec3 colorIn) {
    //return colorIn;
    if (r_toneMap > 0.5) {
        return ACESFitted(colorIn);
        //return reinhard_extended_luminance(colorIn, 5.0f);
        //return uncharted2_filmic(colorIn);
        //return ACES_approx(colorIn);
        //return MGSVToneMap(colorIn);
    } else {
        return colorIn;
    }
}