#include <enpch.hpp>

#include "Engine/Core/GameMath.hpp"

namespace math {

    quat::quat() 
        : x(0), y(0), z(0), w(1) {}
    quat::quat(scalar _x, scalar _y, scalar _z, scalar _w) 
        : x(_x), y(_y), z(_z), w(_w) {}
    quat::quat(scalar _x, scalar _y, scalar _z)
        : x(_x), y(_y), z(_z), w(0) {
        reconstructW();
    }
    quat::quat(const quat& q)
        : x(q.x), y(q.y), z(q.z), w(q.w) {}
    quat::quat(const vec3& xyz, scalar _w) 
        : x(xyz.x), y(xyz.y), z(xyz.z), w(_w) {}

    void quat::reconstructW() {
        /*float t = 1.0f - x * x - y * y - z * z;

        if (t < 0.0f)
            w = 0.0f;
        else
            w = sqrtf(t);*/
        w = sqrtf(fabs(1.0f - x * x - y * y - z * z));
    }

    scalar quat::length_2() const {
        return x*x + y*y + z*z + w*w;
    }

    quat quat::normalize() {
        scalar c = 0, l = sqrtf(x*x + y*y + z*z + w*w);
        if (l > 0.0)
            c = 1 / l;
        x *= c;
        y *= c;
        z *= c;
        w *= c;
        return *this;
    }
    quat quat::inv() const {
        return quat(-x, -y, -z, w);
    }
    vec3 quat::getVectorPart() const {
        return vec3(x, y, z);
    }
    scalar quat::dot(const quat& q) const {
        return x*q.x + x*q.x + x*q.x + x*q.x;
    }
    vec4 quat::asVec4() const {
        return vec4(x, y, z, w);
    }

    const scalar* quat::ptr() const {
        return &x;
    }
    scalar& quat::operator[](std::size_t idx) {
        assert(idx >= 0 && idx < 4);
        return *((&x) + idx);
    }
    const scalar& quat::operator[](std::size_t idx) const {
        assert(idx >= 0 && idx < 4);
        return *((&x) + idx);
    }
    ::std::ostream & operator<<(std::ostream & os, const quat & v)
    {
        // TODO: float formatting
        return os << "quat:(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ") ";
    }

    quat operator* (const quat& q1, const quat& q2) {
        vec3 v1 = q1.getVectorPart();
        vec3 v2 = q2.getVectorPart();
        return quat(v1*q2.w + v2*q1.w + v1.cross(v2), q1.w*q2.w - v1.dot(v2));
    }
}