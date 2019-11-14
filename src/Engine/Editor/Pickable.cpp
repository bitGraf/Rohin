#include "Pickable.hpp"

Pickable::Pickable() {
    pickRadius = 100.0;
    screenPos = vec3(0, 0, 0);
}

void Pickable::calcScreenSpace(mat4 projectionMatrix, mat4 viewMatrix, mat4 modelMatrix, vec3 position, u32 width, u32 height) {
    vec4 pos = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1);

    vec3 sPos = vec3(pos.x / pos.w, pos.y / pos.w, pos.z / pos.w);
    sPos = (sPos*0.5) + vec3(0.5);

    screenPos = vec3(width*(sPos.x), height*(1-sPos.y), sPos.z);
}