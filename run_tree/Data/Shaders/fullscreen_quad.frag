#version 330 core
out vec4 FragColor;

in vec2 tex_coord;
in mat4 lightSpaceMatrix;

struct DirectionalLight {
	vec3 direction;
	vec4 color;
	float strength;
};

uniform sampler2D tex;
uniform sampler2D gPosition;
uniform sampler2D shadowMap;

uniform DirectionalLight sun;

uniform vec3 camPos;

const int ShadowSamples = 1;

float ShadowCalculation(vec4 fragPosLightSpace);
float ComputeScattering(float lightDotView);
const float G_SCATTERING = 0.1;
const float PI = 3.14159265359;

void main() {
    const float gamma = 2.2;
    vec3 hdrColor = texture(tex, tex_coord).rgb;

    vec3 worldPos = texture(gPosition, tex_coord).rgb;
    vec3 startPosition = worldPos;

    vec3 rayVector = camPos - startPosition;
    float rayLength = length(rayVector);
    vec3 rayDirection = normalize(rayVector);

    const int NUM_STEPS = 100;

    float stepLength = rayLength / NUM_STEPS; // 100 steps
    vec3 step = rayDirection*stepLength;

    vec3 currentPosition = startPosition;
    vec3 accumFog = vec3(0,0,0);

    for (int i = 0; i < NUM_STEPS; i++) {
        vec4 lightSpacePos = lightSpaceMatrix * vec4(currentPosition,1);
        float shadow = 1-ShadowCalculation(lightSpacePos);

        //if (shadow < 0.5) {
            accumFog += shadow*ComputeScattering(dot(rayDirection, sun.direction)).xxx * sun.color.rgb * sun.strength;
        //}

        currentPosition += step;
    }
    accumFog /= NUM_STEPS;
    vec4 lightSpacePos = lightSpaceMatrix * vec4(worldPos,1);
    float shadow = 1-ShadowCalculation(lightSpacePos);

    accumFog *= (0.5);

    vec3 colAndFog = hdrColor + accumFog;

    vec3 mapped = colAndFog / (colAndFog + vec3(1.0));
    
    mapped = pow(mapped, vec3(1.0/gamma));

    FragColor = vec4(mapped, 1);
}

float ComputeScattering(float lightDotView) {
    float result = 1.0 - G_SCATTERING * G_SCATTERING;
    result /= (4 * PI * pow(1 + G_SCATTERING*G_SCATTERING - (2*G_SCATTERING) * lightDotView, 1.5));
    return result;
}


float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;
    float bias = 0.005;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -(ShadowSamples/2); x <= ShadowSamples/2; ++x) {
        for (int y = -(ShadowSamples/2); y <= ShadowSamples/2; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y)*texelSize).r;
            shadow += currentDepth-bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= (ShadowSamples*ShadowSamples);

    if (projCoords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}