#ifndef GAME_MATH_H
#define GAME_MATH_H

#include <iostream>

#include "DataTypes.hpp"

namespace math {
    class vec2 {
    public:
        vec2();
        vec2(f32 v);
        vec2(f32 a, f32 b);

        friend std::ostream& operator<<(std::ostream& os, const vec2& v);

        f32 x, y;
    };
    class vec3 {
    public:
        vec3();
        vec3(f32 v);
        vec3(f32 a, f32 b, f32 c);

        friend std::ostream& operator<<(std::ostream& os, const vec3& v);

        f32 x,y,z;
    };

    class mat3 {
    public:
        mat3();
        mat3(f32 v);
        mat3(vec3 v1, vec3 v2, vec3 v3);

        friend std::ostream& operator<<(std::ostream& os, const mat3& m);
    private:
        f32 x[9];
    };

    class mat4 {
    public:
        mat4();

    private:
        f32 x[16];
    };
}

#endif
