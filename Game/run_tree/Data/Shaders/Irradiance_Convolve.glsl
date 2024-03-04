#type vertex
#version 430 core

layout (location = 0) in vec3 a_Position;

out vec3 local_pos;

layout (location = 1) uniform mat4 r_Projection;
layout (location = 2) uniform mat4 r_View;

void main() {
    local_pos = a_Position;  
    gl_Position =  r_Projection * r_View * vec4(local_pos, 1.0);
}

#type fragment
#version 430 core

layout (location = 0) out vec4 FragColor;

in vec3 local_pos;

layout (location = 3) uniform samplerCube u_env_cubemap;

const float PI = 3.14159265359;

void main()
{		
    // the sample direction equals the hemisphere's orientation 
    vec3 normal = normalize(local_pos);
  
    vec3 irradiance = vec3(0.0);
  
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, normal));
    up         = normalize(cross(normal, right));

    float sampleDelta = 0.025;
    float nrSamples = 0.0; 
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

            irradiance += texture(u_env_cubemap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    // OVERRIDE
    irradiance = texture(u_env_cubemap, normal).rgb;
  
    FragColor = vec4(irradiance, 1.0);
}