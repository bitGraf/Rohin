#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>

#include "GameMath.hpp"

std::string getNextString(std::istringstream& iss);
math::scalar getNextFloat(std::istringstream& iss);
math::vec2 getNextVec2(std::istringstream& iss);
math::vec3 getNextVec3(std::istringstream& iss);
math::vec4 getNextVec4(std::istringstream& iss);

#endif
