#include "Light.hpp"

void PointLight::Create(istringstream &iss, ResourceManager* resource) {
    Color = getNextVec3(iss);
    Strength = getNextFloat(iss);
    Position = getNextVec3(iss);

    std::cout << "Light:" << m_uid << "{" << Name << "} created." << endl;
}

void SpotLight::Create(istringstream &iss, ResourceManager* resource) {
    Color = getNextVec3(iss);
    Strength = getNextFloat(iss);
    Position = getNextVec3(iss);

    Direction = getNextVec3(iss);

    vec2 cutoff = getNextVec2(iss);
    inner_cutoff = cos(cutoff.x * d2r);
    outer_cutoff = cos(cutoff.y * d2r);

    std::cout << "Light:" << m_uid << "{" << Name << "} created." << endl;
}

void DirLight::Create(istringstream &iss, ResourceManager* resource) {
    Color = getNextVec3(iss);
    Strength = getNextFloat(iss);
    float distance = getNextFloat(iss);

    Direction = getNextVec3(iss).get_unit();

    Position = -Direction*distance;

    std::cout << "Light:" << m_uid << "{" << Name << "} created." << endl;
}