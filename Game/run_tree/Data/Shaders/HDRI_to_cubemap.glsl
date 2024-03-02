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

layout (location = 3) uniform sampler2D u_hdri;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(local_pos)); // make sure to normalize localPos
    vec3 color = texture(u_hdri, uv).rgb;
    
    FragColor = vec4(color, 1.0);
}