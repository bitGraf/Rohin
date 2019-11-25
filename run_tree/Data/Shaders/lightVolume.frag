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
uniform sampler2D positionMap;
uniform sampler2D shadowMap;

uniform DirectionalLight sun;

uniform vec3 camPos;

const int ShadowSamples = 3;
const int NUM_STEPS = 100;

const mat4 ditherPattern = mat4(
    vec4( 0.0f, 0.5f, 0.125f, 0.625f),
    vec4( 0.75f, 0.22f, 0.875f, 0.375f),
    vec4( 0.1875f, 0.6875f, 0.0625f, 0.5625),
    vec4( 0.9375f, 0.4375f, 0.8125f, 0.3125));

float ShadowCalculation(vec4 fragPosLightSpace);
float ComputeScattering(float lightDotView);
const float G_SCATTERING = 0.1;
const float PI = 3.14159265359;

void main() {
    vec3 worldPos = texture(positionMap, tex_coord).rgb;
    vec3 startPosition = worldPos;

    vec3 rayVector = camPos - startPosition;
    float rayLength = length(rayVector);
    vec3 rayDirection = normalize(rayVector);

    float stepLength = rayLength / NUM_STEPS;
    vec3 step = rayDirection*stepLength;

    vec3 currentPosition = startPosition;
    vec3 accumFog = vec3(0,0,0);

    int screenX = int(tex_coord.x * 800.0);
    int screenY = int(tex_coord.y * 600.0);

    float ditherValue = ditherPattern[screenX % 4][screenY % 4];

    for (int i = 0; i < NUM_STEPS; i++) {
        vec4 lightSpacePos = lightSpaceMatrix * vec4(currentPosition,1);
        float shadow = 1-ShadowCalculation(lightSpacePos);

        float scatter = ComputeScattering(dot(rayDirection, sun.direction));
        accumFog += shadow*vec3(scatter) * sun.color.rgb * sun.strength;

        //currentPosition += step * ditherValue;
        currentPosition += step;
    }
    accumFog /= NUM_STEPS;
    vec4 lightSpacePos = lightSpaceMatrix * vec4(worldPos,1);
    float shadow = 1-ShadowCalculation(lightSpacePos);

    accumFog *= (1);

    vec3 color_in = texture(tex, tex_coord).rgb;
    vec3 color_out = color_in + accumFog;

    FragColor = vec4(color_in, 1);
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