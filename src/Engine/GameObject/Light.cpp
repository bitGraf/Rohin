#include "Light.hpp"
const char* PointLight::_obj_type_PointLightObject = "PointLight";
const char* SpotLight::_obj_type_SpotLightObject = "SpotLight";
const char* DirLight::_obj_type_DirLightObject = "DirLight";

void PointLight::Create(istringstream &iss, ResourceManager* resource) {
    Color = getNextVec3(iss);
    Strength = getNextFloat(iss);
    Position = getNextVec3(iss);

    Console::logMessage("GameObject: %llu {%s} created.", (m_uid), Name.c_str());
}

const char* PointLight::ObjectTypeString() {
    return _obj_type_PointLightObject;
}

void SpotLight::Create(istringstream &iss, ResourceManager* resource) {
    Color = getNextVec3(iss);
    Strength = getNextFloat(iss);
    Position = getNextVec3(iss);

    Direction = getNextVec3(iss);

    vec2 cutoff = getNextVec2(iss);
    inner_cutoff = cos(cutoff.x * d2r);
    outer_cutoff = cos(cutoff.y * d2r);

    Console::logMessage("GameObject: %llu {%s} created.", (m_uid), Name.c_str());
}

const char* SpotLight::ObjectTypeString() {
    return _obj_type_SpotLightObject;
}

void DirLight::Create(istringstream &iss, ResourceManager* resource) {
    Color = getNextVec3(iss);
    Strength = getNextFloat(iss);
    float distance = getNextFloat(iss);

    Direction = getNextVec3(iss).get_unit();

    Position = -Direction*distance;

    Console::logMessage("GameObject: %llu {%s} created.", (m_uid), Name.c_str());
}

const char* DirLight::ObjectTypeString() {
    return _obj_type_DirLightObject;
}