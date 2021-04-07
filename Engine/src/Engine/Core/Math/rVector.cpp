#include <enpch.hpp>

#include "Engine/Core/GameMath.hpp"

namespace math {
    /* * * * * * * * * * * * * * * * * * * * * * * * * * *
     * Vec2  * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * */

    vec2::vec2() : x(0), y(0) {}
    vec2::vec2(scalar f) : x(f), y(f) {}
    vec2::vec2(scalar _x, scalar _y) : x(_x), y(_y) {}
    vec2::vec2(const vec2& v) : x(v.x), y(v.y) {}

    scalar vec2::length() const {
        return sqrt(x*x + y * y);
    }
    scalar vec2::length_2() const {
        return (x*x + y * y);
    }
    vec2 vec2::get_unit() const {
        scalar c = 0, l = length();
        if (l > 0.0)
            c = 1 / l;
        return vec2(x*c, y*c);
    }
    vec2 vec2::normalize() {
        scalar c = 0, l = length();
        if (l > 0.0)
            c = 1 / l;
        x *= c;
        y *= c;
        return *this;
    }
    scalar vec2::dot(const vec2& v) const {
        return (x*v.x + y * v.y);
    }
    const scalar* vec2::ptr() const {
        return &x;
    }
    scalar& vec2::operator[](std::size_t idx) {
        assert(idx >= 0 && idx < 2);
        return *((&x) + idx);
    }
    const scalar& vec2::operator[](std::size_t idx) const {
        assert(idx >= 0 && idx < 2);
        return *((&x) + idx);
    }
    ::std::ostream & operator<<(std::ostream & os, const vec2 & v)
    {
        // TODO: float formatting
        return os << "(" << v.x << ", " << v.y << ") ";
    }

    bool operator== (const vec2& A, const vec2& B) {
        return (A.x == B.x && A.y == B.y);
    }
    vec2 operator+ (const vec2& A, const vec2& B) {
        return vec2(A.x + B.x, A.y + B.y);
    }
    vec2 operator- (const vec2& A, const vec2& B) {
        return vec2(A.x - B.x, A.y - B.y);
    }
    vec2 operator* (const vec2& V, scalar s) {
        return vec2(V.x*s, V.y*s);
    }
    vec2 operator* (scalar s, const vec2& V) {
        return vec2(V.x*s, V.y*s);
    }
    vec2 operator/ (const vec2& V, scalar s) {
        return vec2(V.x / s, V.y / s);
    }
    vec2& operator+= (vec2& A, const vec2& B) {
        A = vec2(A.x + B.x, A.y + B.y);
        return A;
    }
    vec2& operator-= (vec2& A, const vec2& B) {
        A = vec2(A.x - B.x, A.y - B.y);
        return A;
    }
    vec2& operator*= (vec2& V, scalar s) {
        V = vec2(V.x*s, V.y*s);
        return V;
    }
    vec2& operator/= (vec2& V, scalar s) {
        V = vec2(V.x / s, V.y / s);
        return V;
    }
    vec2 operator- (const vec2& V) {
        return vec2(-V.x, -V.y);
    }


    /* * * * * * * * * * * * * * * * * * * * * * * * * * *
     * Vec3  * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * */
    vec3::vec3() : x(0), y(0), z(0) {}
    vec3::vec3(scalar f) : x(f), y(f), z(f) {}
    vec3::vec3(scalar _x, scalar _y, scalar _z) : x(_x), y(_y), z(_z) {}
    vec3::vec3(const vec2& v, scalar _z) : x(v.x), y(v.y), z(_z) {}
    vec3::vec3(const vec3& v) : x(v.x), y(v.y), z(v.z) {}

    scalar vec3::length() const {
        return sqrt(x*x + y * y + z * z);
    }
    scalar vec3::length_2() const {
        return (x*x + y * y + z * z);
    }
    vec3 vec3::get_unit() const {
        scalar c = 0, l = length();
        if (l > 0.0)
            c = 1 / l;
        return vec3(x*c, y*c, z*c);
    }
    vec3 vec3::normalize() {
        scalar c = 0, l = length();
        if (l > 0.0)
            c = 1 / l;
        x *= c;
        y *= c;
        z *= c;
        return *this;
    }
    scalar vec3::dot(const vec3& v) const {
        return (x*v.x + y * v.y + z * v.z);
    }
    vec3 vec3::cross(const vec3& v) const {
        return vec3(y*v.z - z * v.y, z*v.x - x * v.z, x*v.y - y * v.x);
    }
    vec2 vec3::asVec2() const {
        return vec2(x, y);
    }
    const scalar* vec3::ptr() const {
        return &x;
    }
    scalar& vec3::operator[](std::size_t idx) {
        assert(idx >= 0 && idx < 3);
        return *((&x) + idx);
    }
    const scalar& vec3::operator[](std::size_t idx) const {
        assert(idx >= 0 && idx < 3);
        return *((&x) + idx);
    }
    ::std::ostream & operator<<(std::ostream & os, const vec3 & v)
    {
        // TODO: float formatting
        return os << "(" << v.x << ", " << v.y << ", " << v.z << ") ";
    }

    bool operator== (const vec3& A, const vec3& B) {
        return (A.x == B.x && A.y == B.y && A.z == B.z);
    }
    vec3 operator+ (const vec3& A, const vec3& B) {
        return vec3(A.x + B.x, A.y + B.y, A.z + B.z);
    }
    vec3 operator- (const vec3& A, const vec3& B) {
        return vec3(A.x - B.x, A.y - B.y, A.z - B.z);
    }
    vec3 operator* (const vec3& V, scalar s) {
        return vec3(V.x*s, V.y*s, V.z*s);
    }
    vec3 operator* (scalar s, const vec3& V) {
        return vec3(V.x*s, V.y*s, V.z*s);
    }
    vec3 operator/ (const vec3& V, scalar s) {
        return vec3(V.x / s, V.y / s, V.z / s);
    }
    vec3& operator+= (vec3& A, const vec3& B) {
        A = vec3(A.x + B.x, A.y + B.y, A.z + B.z);
        return A;
    }
    vec3& operator-= (vec3& A, const vec3& B) {
        A = vec3(A.x - B.x, A.y - B.y, A.z - B.z);
        return A;
    }
    vec3& operator*= (vec3& V, scalar s) {
        V = vec3(V.x*s, V.y*s, V.z*s);
        return V;
    }
    vec3& operator/= (vec3& V, scalar s) {
        V = vec3(V.x / s, V.y / s, V.z / s);
        return V;
    }
    vec3 operator- (const vec3& V) {
        return vec3(-V.x, -V.y, -V.z);
    }


    /* * * * * * * * * * * * * * * * * * * * * * * * * * *
     * Vec4  * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * */
    vec4::vec4() : x(0), y(0), z(0), w(0) {}
    vec4::vec4(scalar f) : x(f), y(f), z(f), w(f) {}
    vec4::vec4(scalar _x, scalar _y, scalar _z, scalar _w) : x(_x), y(_y), z(_z), w(_w) {}

    vec4::vec4(const vec2& v, scalar _z, scalar _w) : x(v.x), y(v.y), z(_z), w(_w) {}
    vec4::vec4(const vec3& v, scalar _w) : x(v.x), y(v.y), z(v.z), w(_w) {}
    vec4::vec4(const vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    scalar vec4::length() const {
        return sqrt(x*x + y * y + z * z + w * w);
    }
    scalar vec4::length_2() const {
        return (x*x + y * y + z * z + w * w);
    }
    vec4 vec4::get_unit() const {
        scalar c = 0, l = length();
        if (l > 0.0)
            c = 1 / l;
        return vec4(x*c, y*c, z*c, w*c);
    }
    vec4 vec4::normalize() {
        scalar c = 0, l = length();
        if (l > 0.0)
            c = 1 / l;
        x *= c;
        y *= c;
        z *= c;
        w *= c;
        return *this;
    }
    scalar vec4::dot(const vec4& v) const {
        return (x*v.x + y * v.y + z * v.z + w * v.w);
    }
    vec2 vec4::asVec2() const {
        return vec2(x, y);
    }
    vec3 vec4::asVec3() const {
        return vec3(x, y, z);
    }
    const scalar* vec4::ptr() const {
        return &x;
    }
    scalar& vec4::operator[](std::size_t idx) {
        assert(idx >= 0 && idx < 4);
        return *((&x) + idx);
    }
    const scalar& vec4::operator[](std::size_t idx) const {
        assert(idx >= 0 && idx < 4);
        return *((&x) + idx);
    }
    ::std::ostream & operator<<(std::ostream & os, const vec4 & v)
    {
        // TODO: float formatting
        return os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ") ";
    }

    bool operator== (const vec4& A, const vec4& B) {
        return (A.x == B.x && A.y == B.y && A.z == B.z && A.w == B.w);
    }
    vec4 operator+ (const vec4& A, const vec4& B) {
        return vec4(A.x + B.x, A.y + B.y, A.z + B.z, A.w + B.w);
    }
    vec4 operator- (const vec4& A, const vec4& B) {
        return vec4(A.x - B.x, A.y - B.y, A.z - B.z, A.w - B.w);
    }
    vec4 operator* (const vec4& V, scalar s) {
        return vec4(V.x*s, V.y*s, V.z*s, V.w*s);
    }
    vec4 operator* (scalar s, const vec4& V) {
        return vec4(V.x*s, V.y*s, V.z*s, V.w*s);
    }
    vec4 operator/ (const vec4& V, scalar s) {
        return vec4(V.x / s, V.y / s, V.z / s, V.w / s);
    }
    vec4& operator+= (vec4& A, const vec4& B) {
        A = vec4(A.x + B.x, A.y + B.y, A.z + B.z, A.w + B.w);
        return A;
    }
    vec4& operator-= (vec4& A, const vec4& B) {
        A = vec4(A.x - B.x, A.y - B.y, A.z - B.z, A.w - B.w);
        return A;
    }
    vec4& operator*= (vec4& V, scalar s) {
        V = vec4(V.x*s, V.y*s, V.z*s, V.w*s);
        return V;
    }
    vec4& operator/= (vec4& V, scalar s) {
        V = vec4(V.x / s, V.y / s, V.z / s, V.w / s);
        return V;
    }
    vec4 operator- (const vec4& V) {
        return vec4(-V.x, -V.y, -V.z, -V.w);
    }
}