#include "Light.hpp"
const char* PointLight::_obj_type_PointLightObject = "PointLight";
const char* SpotLight::_obj_type_SpotLightObject = "SpotLight";
const char* DirLight::_obj_type_DirLightObject = "DirLight";

void PointLight::Create(DataNode* node) {
    Color = node->getVec3("color_r", "color_g", "color_b", vec3(1));
    Strength = node->getData("strength").asFloat(1);
    Position = node->getVec3("posx", "posy", "posz");

    Console::logMessage("GameObject: %llu {%s} created.", (m_uid), Name.c_str());
}

const char* PointLight::ObjectTypeString() {
    return _obj_type_PointLightObject;
}

void SpotLight::Create(DataNode* node) {
    Color = node->getVec3("color_r", "color_g", "color_b", vec3(1));
    Strength = node->getData("strength").asFloat(1);
    Position = node->getVec3("posx", "posy", "posz");

    Direction = node->getVec3("dirx", "diry", "dirz").normalize();

    vec2 cutoff = node->getVec2("cutoff_inner", "cutoff_outer", vec2(.1, .2));
    inner_cutoff = cos(cutoff.x * d2r);
    outer_cutoff = cos(cutoff.y * d2r);

    Console::logMessage("GameObject: %llu {%s} created.", (m_uid), Name.c_str());
}

const char* SpotLight::ObjectTypeString() {
    return _obj_type_SpotLightObject;
}

void DirLight::Create(DataNode* node) {
    Color = node->getVec3("color_r", "color_g", "color_b", vec3(1));
    Strength = node->getData("strength").asFloat(1);
    Position = node->getVec3("posx", "posy", "posz");

    Direction = node->getVec3("dirx", "diry", "dirz", vec3(1,0,0)).normalize();

    Console::logMessage("GameObject: %llu {%s} created.", (m_uid), Name.c_str());
}

const char* DirLight::ObjectTypeString() {
    return _obj_type_DirLightObject;
}