#version 330 core
out vec4 FragColor;

in vec3 pass_normal;
in vec3 pass_fragPos;
in vec4 pass_fragPosLightSpace;
in vec2 pass_tex;
in mat4 pass_lightSpaceMatrix;

struct DirectionalLight {
	vec3 direction;
	vec4 color;
	float strength;
};

uniform DirectionalLight sun;
uniform vec3 camPos;

uniform sampler2D   shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

void main()
{
    vec3 V = normalize(camPos - pass_fragPos);
    float inShadow = ShadowCalculation(pass_fragPosLightSpace, pass_normal, sun.direction);

    vec3 startPos = pass_fragPos;
    vec3 endPos = camPos;
    float distToCam = length(endPos - startPos);
    vec3 toCam = normalize(endPos - startPos);

    const int numSteps = 5;
    float stepLength = distToCam / numSteps;
    float numThatCanSeeLight = 0;
    for (int n = 0; n < numSteps; n++) {
        vec3 samplePos = startPos + toCam*stepLength*n;
        vec4 samplePosLS = (pass_lightSpaceMatrix * vec4(samplePos,1));
        float sampleInShadow = 1-ShadowCalculation(samplePosLS, pass_normal, sun.direction);

        numThatCanSeeLight += sampleInShadow;
    }

    float colOut = numThatCanSeeLight / 5.0;

    FragColor = vec4(vec3(.8,.12,.34), 1-inShadow);
    FragColor = vec4(vec3(colOut), colOut);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    int ShadowSamples = 1;
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