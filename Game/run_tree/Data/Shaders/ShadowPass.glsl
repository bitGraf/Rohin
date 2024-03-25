#type vertex
#version 430 core

layout (location = 0) in vec3 a_Position;

// can combine these two into ModelView matrix
layout (location = 1) uniform mat4 r_Transform;

layout (location = 2) uniform mat4 r_LightSpace;

void main() {
    gl_Position = r_LightSpace * r_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 430 core

layout (location = 0) out vec4 out_Depth;    //RGBA32F

void main()
{
    // write to render targets
    out_Depth = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
    //out_Depth = gl_FragCoord.z;
}