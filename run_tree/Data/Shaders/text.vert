#version 330 core
layout (location = 0) in vec2 vertPos;

out vec2 pass_uv;

uniform mat4 projectionMatrix;
uniform vec4 transform;
uniform vec4 transformUV;

void main() {

    vec2 newPos = vec2(transform.x   * vertPos.x, transform.y   * vertPos.y) + transform.zw;
    vec2 newUV  = vec2(transformUV.x * vertPos.x, transformUV.y * vertPos.y) + vec2(transformUV.z, transformUV.w);

    vec4 pos = projectionMatrix * vec4(newPos.x, newPos.y, -1.0, 1.0);

    gl_Position = pos;
    //gl_Position = pos.xyww;
    //gl_Position.z = 0.5;
    pass_uv = newUV;
}