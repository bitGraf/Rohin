#type vertex
#version 430 core
layout (location = 0) in vec3 a_Position;

out vec3 tex_coord;

layout (location = 1) uniform mat4 r_View;
layout (location = 2) uniform mat4 r_Projection;

void main() {
    tex_coord = a_Position;
    vec4 pos = r_Projection * r_View * vec4(a_Position, 1.0);
    //gl_Position = pos.xyww;
    gl_Position = pos;
}

#type fragment
#version 430 core
out vec4 FragColor;

in vec3 tex_coord;

layout (location = 3) uniform samplerCube u_skybox;

layout (location = 4) uniform float r_toneMap;
layout (location = 5) uniform float r_gammaCorrect;

vec3 ToneMap(vec3 colorIn);

void main() {
    FragColor = texture(u_skybox, tex_coord);

    FragColor = vec4(ToneMap(FragColor.rgb), 1.0);

    // Gamma correct
    float gamma = 2.2;
    if (r_gammaCorrect > 0.5)
        FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
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

vec3 ACESFitted(vec3 color) {
    color = transpose(ACESInputMat) * color;

    // Apply RRT and ODT
    color = RRTAndODTFit(color);

    color = transpose(ACESOutputMat) * color;

    // Clamp to [0,1]
    color = clamp(color, 0.0, 1.0);

    return color;
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