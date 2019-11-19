#version 330 core
layout (location = 0) in vec2 vertPos;

out vec2 pass_uv;

uniform vec2 pos;
uniform vec2 scale;

void main() {
    vec2 adjScale = scale * vec2(1,4.0/3.0); //adjust for aspect ratio. Temp fix
    vec2 scaledPos = vertPos * adjScale;
    vec2 mappedPos = vec2(scaledPos.x, 1-scaledPos.y);
    vec2 translatedPos = mappedPos + vec2(pos.x,-pos.y) + vec2(-adjScale.x/2, adjScale.y/2);
    vec2 ndcPos = 2 * (translatedPos - vec2(.5));

    gl_Position = vec4(ndcPos,0,1);
    pass_uv = vec2(vertPos.x, 1-vertPos.y);
}