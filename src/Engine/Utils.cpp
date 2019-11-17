#include "Utils.hpp"

std::string getNextString(std::istringstream& iss) {
    std::string str;
    std::getline(iss, str, '"');
    std::getline(iss, str, '"');

    return str;
}

math::scalar getNextFloat(std::istringstream& iss) {
    math::scalar v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v;
    std::getline(iss, garb, '"');

    return v;
}

math::vec2 getNextVec2(std::istringstream& iss) {
    math::vec2 v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v.x >> v.y;
    std::getline(iss, garb, '"');

    return v;
}

math::vec3 getNextVec3(std::istringstream& iss) {
    math::vec3 v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v.x >> v.y >> v.z;
    std::getline(iss, garb, '"');

    return v;
}

math::vec4 getNextVec4(std::istringstream& iss) {
    math::vec4 v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v.x >> v.y >> v.z >> v.w;
    std::getline(iss, garb, '"');

    return v;
}