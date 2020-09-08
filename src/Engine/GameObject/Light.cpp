#include "Light.hpp"
const char* PointLight::_obj_type_PointLightObject = "PointLight";
const char* SpotLight::_obj_type_SpotLightObject = "SpotLight";
const char* DirLight::_obj_type_DirLightObject = "DirLight";

void PointLight::Create(DataNode* node) {
    Color = node->getData("color").asVec3(vec3(1));
    Strength = node->getData("strength").asFloat(1);
    Position = node->getData("position").asVec3(vec3());

    Console::logMessage("GameObject: %llu {%s} created.", (m_uid), Name.c_str());
}

const char* PointLight::ObjectTypeString() {
    return _obj_type_PointLightObject;
}

void SpotLight::Create(DataNode* node) {
    Color = node->getData("color").asVec3(vec3(1));
    Strength = node->getData("strength").asFloat(1);
    Position = node->getData("position").asVec3(vec3());

    Direction = node->getData("direction").asVec3(vec3());

    vec2 cutoff = node->getData("cutoff").asVec2(vec2(.1, .2));
    inner_cutoff = cos(cutoff.x * d2r);
    outer_cutoff = cos(cutoff.y * d2r);

    Console::logMessage("GameObject: %llu {%s} created.", (m_uid), Name.c_str());
}

const char* SpotLight::ObjectTypeString() {
    return _obj_type_SpotLightObject;
}

void DirLight::Create(DataNode* node) {
    Color = node->getData("color").asVec3(vec3(1));
    Strength = node->getData("strength").asFloat(1);
    Position = node->getData("position").asVec3(vec3());

    Direction = node->getData("direction").asVec3(vec3(1,0,0)).normalize();

    Console::logMessage("GameObject: %llu {%s} created.", (m_uid), Name.c_str());
}

const char* DirLight::ObjectTypeString() {
    return _obj_type_DirLightObject;
}