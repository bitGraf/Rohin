#include <enpch.hpp>

#include "Engine/Core/GameMath.hpp"

namespace math {

    void test____quat_transform() {
        quat q;
        vec3 v;
        
        vec3 t = 2 * q.getVectorPart().cross(v);
        vec3 vp = v + q.w*t + q.getVectorPart().cross(t);
    }

    /* Transformation helpers */
    vec3 TransformPointByQuaternion(const quat& rotation, const vec3& point) {
        return ((rotation*quat(point, 0))*rotation.inv()).getVectorPart();
    }

    vec3 TransformPointByMatrix4x4(const mat4& transform, const vec3& point) {
        return (transform * vec4(point, 1.0f)).asVec3();
    }

    void CreateOrthoProjection(mat4& matrix, scalar left, scalar right, scalar bottom, scalar top, scalar znear, scalar zfar) {
        matrix.identity();
        matrix[0][0] =  2.0 / (right - left);
        matrix[1][1] =  2.0 / (top - bottom);
        matrix[2][2] = -2.0 / (zfar - znear);

        matrix[3][0] = -(right + left) / (right - left);
        matrix[3][1] = -(top + bottom) / (top - bottom);
        matrix[3][2] = -(zfar + znear) / (zfar - znear);
    }

    void CreateRotationFromYawPitchRoll(mat3& matrix, scalar yaw, scalar pitch, scalar roll) {
        scalar C1, C2, C3, S1, S2, S3;
        C1 = cos(yaw * d2r);
        C2 = cos(pitch * d2r);
        C3 = cos(roll * d2r);
        S1 = sin(yaw * d2r);
        S2 = sin(pitch * d2r);
        S3 = sin(roll * d2r);
        
        matrix.identity();
        matrix[0][0] = C1*C3 - S1*S2*S3;
        matrix[0][1] = -C2 * S3;
        matrix[0][2] = -S1*C3 - C1*S2*S3;

        matrix[1][0] = C1*S3 + S1*S2*C3;
        matrix[1][1] = C2*C3;
        matrix[1][2] = -S1*S3 + C1*S2*C3;

        matrix[2][0] = S1*C2;
        matrix[2][1] = -S2;
        matrix[2][2] = C1*C2;
    }
    void CreateRotationFromYawPitchRoll(mat4& matrix, scalar yaw, scalar pitch, scalar roll) {
        scalar C1, C2, C3, S1, S2, S3;
        C1 = cos(yaw * d2r);
        C2 = cos(pitch * d2r);
        C3 = cos(roll * d2r);
        S1 = sin(yaw * d2r);
        S2 = sin(pitch * d2r);
        S3 = sin(roll * d2r);

        matrix.identity();
        matrix[0][0] = C1*C3 - S1*S2*S3;
        matrix[0][1] = -C2*S3;
        matrix[0][2] = -S1*C3 - C1*S2*S3;
        matrix[0][3] = 0;

        matrix[1][0] = C1*S3 + S1*S2*C3;
        matrix[1][1] = C2*C3;
        matrix[1][2] = -S1*S3 + C1*S2*C3;
        matrix[1][3] = 0;

        matrix[2][0] = S1*C2;
        matrix[2][1] = -S2;
        matrix[2][2] = C1*C2;
        matrix[2][3] = 0;

        matrix[3][0] = 0;
        matrix[3][1] = 0;
        matrix[3][2] = 0;
        matrix[3][3] = 1;
    }

    void CreateRotationFromYawPitch(mat3& matrix, scalar yaw, scalar pitch) {
        CreateRotationFromYawPitchRoll(matrix, yaw, pitch, 0);
    }
    void CreateRotationFromYawPitch(mat4& matrix, scalar yaw, scalar pitch) {
        CreateRotationFromYawPitchRoll(matrix, yaw, pitch, 0);
    }

    void CreateRotationFromQuaternion(mat3& matrix, const quat& q) {
        matrix.identity();
        matrix[0][0] = 1 - 2 * (q.y*q.y) - 2 * (q.z*q.z);
        matrix[0][1] = 2 * (q.x*q.y + q.z*q.w);
        matrix[0][2] = 2 * (q.z*q.x - q.y*q.w);
        matrix[1][0] = 2 * (q.x*q.y - q.z*q.w);
        matrix[1][1] = 1 - 2 * (q.z*q.z) - 2 * (q.x*q.x);
        matrix[1][2] = 2 * (q.y*q.z + q.x*q.w);
        matrix[2][0] = 2 * (q.z*q.x + q.y*q.w);
        matrix[2][1] = 2 * (q.y*q.z - q.x*q.w);
        matrix[2][2] = 1 - 2*(q.x*q.x) - 2*(q.y*q.y);
    }
    void CreateRotationFromQuaternion(mat4& matrix, const quat& q) {
        matrix.identity();
        matrix[0][0] = 1 - 2 * (q.y*q.y) - 2 * (q.z*q.z);
        matrix[1][0] = 2 * (q.x*q.y - q.z*q.w);
        matrix[2][0] = 2 * (q.z*q.x + q.y*q.w);
        //matrix[3][0] = 0;

        matrix[0][1] = 2 * (q.x*q.y + q.z*q.w);
        matrix[1][1] = 1 - 2 * (q.z*q.z) - 2 * (q.x*q.x);
        matrix[2][1] = 2 * (q.y*q.z - q.x*q.w);
        //matrix[3][1] = 0;

        matrix[0][2] = 2 * (q.z*q.x - q.y*q.w);
        matrix[1][2] = 2 * (q.y*q.z + q.x*q.w);
        matrix[2][2] = 1 - 2 * (q.x*q.x) - 2 * (q.y*q.y);
        //matrix[3][2] = 0;

        //matrix[0][3] = 0;
        //matrix[1][3] = 0;
        //matrix[2][3] = 0;
        //matrix[3][3] = 1;
    }

    void CreateRotationFromAxisAngle(quat& rotation, const vec3& axis, scalar angle) {
        rotation = quat(axis.get_unit() * sin(angle * d2r * 0.5f),cos(angle * d2r * 0.5f));
    }

    void CreateTranslation(mat4& matrix, const vec3& translation) {
        matrix.identity();
        matrix[3][0] = translation.x;
        matrix[3][1] = translation.y;
        matrix[3][2] = translation.z;
    }

    void CreateScale(mat3& matrix, float scaleX, float scaleY, float scaleZ) {
        matrix.identity();
        matrix[0][0] = scaleX;
        matrix[1][1] = scaleY;
        matrix[2][2] = scaleZ;
    }
    void CreateScale(mat4& matrix, float scaleX, float scaleY, float scaleZ) {
        matrix.identity();
        matrix[0][0] = scaleX;
        matrix[1][1] = scaleY;
        matrix[2][2] = scaleZ;
    }
    void CreateScale(mat4& matrix, const vec3& scale) {
        matrix.identity();
        matrix[0][0] = scale.x;
        matrix[1][1] = scale.y;
        matrix[2][2] = scale.z;
    }

    void CreateTransform(mat4& matrix, const vec3& translation, scalar yaw, scalar pitch) {
        // Apply Translation
        CreateTranslation(matrix, translation);

        // Apply Rotation
        mat4 rotM;
        CreateRotationFromYawPitch(rotM, yaw, pitch);
        matrix *= rotM;
    }
    void CreateTransform(mat4& matrix, const mat3& rotation, const vec3& translation) {
        // Apply Translation
        CreateTranslation(matrix, translation);

        // Apply rotation
        mat4 rotM(rotation, 1);
        matrix *= rotM;
    }
    void CreateTransform(mat4& matrix, const mat3& rotation, const vec3& translation, const vec3& scale) {
        // Apply Translation
        CreateTranslation(matrix, translation);

        // Apply rotation
        mat4 rotM(rotation, 1);
        matrix *= rotM;

        // Apply scale
        mat4 scaleM;
        CreateScale(scaleM, scale);
        matrix *= scaleM;
    }
    void CreateTransform(mat4& matrix, const quat& rotation, const vec3& translation, const vec3& scale) {
        // Apply Translation
        CreateTranslation(matrix, translation);

        // Apply rotation
        mat4 rotM;
        CreateRotationFromQuaternion(rotM, rotation);
        matrix *= rotM;

        // Apply scale
        mat4 scaleM;
        CreateScale(scaleM, scale);
        matrix *= scaleM;
    }

    void CreateViewFromTransform(mat4& matrix, const mat4& transform) {
        // Apply inverse rotation
        matrix = mat4(transform.asMat3(), 1);
        matrix.transpose();

        // Apply inverse Translation
        mat4 TransM;
        CreateTranslation(TransM, -(transform.column4.asVec3()));
        matrix *= TransM;
    }

    void Decompose(const mat4& transform, vec3& translation) {
        translation = transform.column4.asVec3();
    }
    void Decompose(const mat4& transform, quat& rotation) {
        rotation.w = 0.5f * sqrtf(1 + transform[0][0] + transform[1][1] + transform[2][2]);
        scalar f = 1.0f / (4.0f * rotation.w);
        
        rotation.x = (transform[2][3] - transform[3][2]) * f;
        rotation.y = (transform[3][1] - transform[1][3]) * f;
        rotation.z = (transform[1][2] - transform[2][1]) * f;
    }
    void Decompose(const mat4& transform, scalar& yaw, scalar& pitch) {
        //ENGINE_LOG_WARN("Decompose transform into yaw/pitch not implemented yet!");
        // Assume no roll -_-
        yaw = acos(transform[0][0]) * r2d;
        pitch = acos(transform[1][1]) * r2d;
    }
    void Decompose(const mat4& transform, scalar& yaw, scalar& pitch, scalar& roll) {
        ENGINE_LOG_WARN("Decompose transform into yaw/pitch/roll not implemented yet!");
    }
    void Decompose(const mat4& transform, vec3& Forward, vec3& Right, vec3& Up) {
        // Assuming no scale -_-
        Right   =  transform.column1.asVec3();
        Up      =  transform.column2.asVec3();
        Forward = -transform.column3.asVec3();
    }
    void Decompose(const mat4& transform, vec3& Translation, quat& Rotation, vec3& Scale) {
        ENGINE_LOG_WARN("Decompose transform into T/R/S not implemented yet!");
    }

    /* Interpolation functions */
    scalar lerp(scalar a, scalar b, scalar f) {
        return a + f * (b - a);
    }
    vec2 lerp(const vec2& a, const vec2& b, scalar f) {
        return vec2(
            a.x + f * (b.x - a.x),
            a.y + f * (b.y - a.y));
    }
    vec3 lerp(const vec3& a, const vec3& b, scalar f) {
        return vec3(
            a.x + f * (b.x - a.x),
            a.y + f * (b.y - a.y),
            a.z + f * (b.z - a.z));
    }
    vec4 lerp(const vec4& a, const vec4& b, scalar f) {
        return vec4(
            a.x + f * (b.x - a.x),
            a.y + f * (b.y - a.y),
            a.z + f * (b.z - a.z),
            a.w + f * (b.w - a.w));
    }
    quat lerp(const quat& q1, const quat& q2, scalar f) {
        return quat(
            q1.x + f * (q2.x - q1.x),
            q1.y + f * (q2.y - q1.y),
            q1.z + f * (q2.z - q1.z),
            q1.w + f * (q2.w - q1.w));
    }

    quat fast_slerp(const quat& q1, const quat& q2, scalar f) {
        // linear interp and renormalize
        quat res = lerp(q1, q2, f);
        res.normalize();
        return res;
    }
    quat slerp(const quat& q1, const quat& q2, scalar f) {
        scalar omega = acos(q1.dot(q2));
        scalar s_omega_inv = 1.0 / sin(omega);
        vec4 v = (sin((1 - f)*omega)*s_omega_inv * q1.asVec4()) + (sin(f*omega)*s_omega_inv * q2.asVec4());
        return quat(v.x, v.y, v.z, v.w);
    }
}