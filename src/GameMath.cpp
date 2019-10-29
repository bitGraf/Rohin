#include "GameMath.hpp"


math::vec2::vec2() {
    x = 0;
    y = 0;
}

math::vec2::vec2(f32 v) {
    x = v;
    y = v;
}

math::vec2::vec2(f32 a, f32 b) {
    x = a;
    y = b;
}

math::vec3::vec3() {
    x = 0;
    y = 0;
    z = 0;
}

math::vec3::vec3(f32 v) {
    x = v;
    y = v;
    z = v;
}

math::vec3::vec3(f32 a, f32 b, f32 c) {
    x = a;
    y = b;
    z = c;
}

math::mat3::mat3() {
    x[0] = 0;
    x[1] = 0;
    x[2] = 0;
    x[3] = 0;
    x[4] = 0;
    x[5] = 0;
    x[6] = 0;
    x[7] = 0;
    x[8] = 0;
}

math::mat3::mat3(f32 v) {
    x[0] = v;
    x[1] = v;
    x[2] = v;
    x[3] = v;
    x[4] = v;
    x[5] = v;
    x[6] = v;
    x[7] = v;
    x[8] = v;
}

math::mat3::mat3(vec3 v1, vec3 v2, vec3 v3) {
    x[0] = v1.x;
    x[1] = v1.y;
    x[2] = v1.z;
    x[3] = v2.x;
    x[4] = v2.y;
    x[5] = v2.z;
    x[6] = v3.x;
    x[7] = v3.y;
    x[8] = v3.z;
}

math::mat4::mat4() {
    x[0] = 0;
    x[1] = 0;
    x[2] = 0;
    x[3] = 0;
    x[4] = 0;
    x[5] = 0;
    x[6] = 0;
    x[7] = 0;
    x[8] = 0;
    x[9] = 0;
    x[10] = 0;
    x[11] = 0;
    x[12] = 0;
    x[13] = 0;
    x[14] = 0;
    x[15] = 0;
}

std::ostream & math::operator<<(std::ostream & os, const vec2 & v)
{
    // TODO: float formatting
    os << "(" << v.x << ", " << v.y << ") ";
    return os;
}

std::ostream & math::operator<<(std::ostream & os, const vec3 & v)
{
    // TODO: float formatting
    os << "(" << v.x << ", " << v.y << ", " << v.z << ") ";
    return os;
}

std::ostream & math::operator<<(std::ostream & os, const mat3 & m)
{
    // TODO: float formatting
    os << "\n/" << m.x[0] << " " << m.x[1] << " " << m.x[2] << "\\";
    return os;
}
