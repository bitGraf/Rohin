#include "Light.hpp"

void PointLight::Create(istringstream &iss, ResourceManager* resource) {
    Color = getNextVec3(iss);
    Strength = getNextFloat(iss);
    Position = getNextVec3(iss);

    Console::logMessage("GameObject: %llu {%s} created.", (m_uid), Name.c_str());
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

void DirLight::Create(istringstream &iss, ResourceManager* resource) {
    Color = getNextVec3(iss);
    Strength = getNextFloat(iss);
    float distance = getNextFloat(iss);

    Direction = getNextVec3(iss).get_unit();

    Position = -Direction*distance;

    Console::logMessage("GameObject: %llu {%s} created.", (m_uid), Name.c_str());
}