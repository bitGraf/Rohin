#include "rhpch.hpp"
#include "Light.hpp"
const char* PointLight::_obj_type_PointLightObject = "PointLight";
const char* SpotLight::_obj_type_SpotLightObject = "SpotLight";
const char* DirLight::_obj_type_DirLightObject = "DirLight";

void PointLight::Create(jsonObj node) {
    Color = safeAccessVec<vec3>(node, "color", vec3(1,1,1));
    Strength = safeAccess<double>(node, "strength", 1.0);
    Position = safeAccessVec<vec3>(node, "position", vec3());

    LOG_INFO("GameObject: %llu {%s} created.", (m_uid), Name.c_str());
}

const char* PointLight::ObjectTypeString() {
    return _obj_type_PointLightObject;
}

void SpotLight::Create(jsonObj node) {
    Color = safeAccessVec<vec3>(node, "color", vec3(1, 1, 1));
    Strength = safeAccess<double>(node, "strength", 1.0);
    Position = safeAccessVec<vec3>(node, "position", vec3());

    Direction = safeAccessVec<vec3>(node, "direction", vec3());

    vec2 cutoff = safeAccessVec<vec2>(node, "cutoff", vec2());
    inner_cutoff = cos(cutoff.x * d2r);
    outer_cutoff = cos(cutoff.y * d2r);

    LOG_INFO("GameObject: %llu {%s} created.", (m_uid), Name.c_str());
}

const char* SpotLight::ObjectTypeString() {
    return _obj_type_SpotLightObject;
}

void DirLight::Create(jsonObj node) {
    Color = safeAccessVec<vec3>(node, "color", vec3(1, 1, 1));
    Strength = safeAccess<double>(node, "strength", 1.0);
    Position = safeAccessVec<vec3>(node, "position", vec3());

    Direction = safeAccessVec<vec3>(node, "direction", vec3(1, 0, 0));

    LOG_INFO("GameObject: %llu {%s} created.", (m_uid), Name.c_str());
}

const char* DirLight::ObjectTypeString() {
    return _obj_type_DirLightObject;
}