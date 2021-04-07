#include <enpch.hpp>
#include "Engine/Core/GameMath.hpp"

namespace math {

    /*
    // Look at
    void mat4::lookAt(vec3 pos, vec3 targ, vec3 Up) {
        vec3 forward = (targ - pos).get_unit();  // +X
        vec3 right;
        if (abs(forward.dot(Up)) > 0.9999) {
            right = forward.cross(vec3(0, 0, 1));      // +Z
        }
        else {
            right = forward.cross(Up);      // +Z
        }
        vec3 up = right.cross(forward);                 // +Y

        mat3 rot = mat3(forward, up, right);

        mat4 m = mat4(vec4(0, 0, -1, 0), vec4(0, 1, 0, 0), vec4(1, 0, 0, 0), vec4(0, 0, 0, 1)) *
            (mat4(rot.getTranspose()) *
                mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(-pos, 1)));
        //mat4 m = (mat4(rot.getTranspose()) *
        //    mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(-pos, 1)));

        *this = m;
    }

    void mat4::orthoProjection(scalar left, scalar right, scalar bottom, scalar top, scalar znear, scalar zfar) {

        *this = mat4(
            vec4(2.0 / (right - left),
                0,
                0,
                0),

            vec4(0,
                2.0 / (top - bottom),
                0,
                0),

            vec4(0,
                0,
                -2.0 / (zfar - znear),
                0),

            vec4(-(right + left) / (right - left),
                -(top + bottom) / (top - bottom),
                -(zfar + znear) / (zfar - znear),
                1.0));
    }

    void mat4::translate(vec3 translation_vector) {
        vec3 t = translation_vector;
        mat4 translationMat = mat4(
            vec4(1, 0, 0, 0),
            vec4(0, 1, 0, 0),
            vec4(0, 0, 1, 0),
            vec4(t.x, t.y, t.z, 1));
        *this = *this*translationMat;
    }

    void mat4::rotate(float angle, vec3 rotation_axis) {
        vec3 r = rotation_axis;
        mat4 rotationMat = mat4(
            vec4(cos(angle) + (pow(r.x, 2)*(1 - cos(angle))),
            (r.y*r.x*(1 - cos(angle))) + (r.z*sin(angle)),
                (r.z*r.x*(1 - cos(angle))) - (r.y*sin(angle)),
                0),
            vec4((r.x*r.y*(1 - cos(angle))) - (r.z*sin(angle)),
                cos(angle) + (pow(r.y, 2)*(1 - cos(angle))),
                (r.z*r.y*(1 - cos(angle))) + (r.x*sin(angle)),
                0),
            vec4((r.x*r.z*(1 - cos(angle))) + (r.y*sin(angle)),
            (r.y*r.z*(1 - cos(angle))) - (r.x*sin(angle)),
                cos(angle) + (pow(r.z, 2)*(1 - cos(angle))),
                0),
            vec4(0,
                0,
                0,
                1));
        *this = *this*rotationMat;
    }

    void mat4::scale(vec3 scale_vector) {
        vec3 s = scale_vector;
        mat4 scaleMat = mat4(
            vec4(s.x, 0, 0, 0),
            vec4(0, s.y, 0, 0),
            vec4(0, 0, s.z, 0),
            vec4(0, 0, 0, 1));
        *this = *this*scaleMat;
    }



    mat3& mat3::toYawPitchRoll(scalar yaw, scalar pitch, scalar roll) {
        // 2-3-1 Body-Fixed Euler-Rotation
        scalar C1 = cos(roll*d2r);  //x Roll
        scalar S1 = sin(roll*d2r);
        scalar C2 = cos(pitch*d2r); //z Pitch
        scalar S2 = sin(pitch*d2r);
        scalar C3 = cos(yaw*d2r);   //y Yaw
        scalar S3 = sin(yaw*d2r);

        mat3 X1 = mat3(
            vec3(1, 0, 0),
            vec3(0, C1, S1),
            vec3(0, -S1, C1)
        );
        mat3 Z2 = mat3(
            vec3(C2, S2, 0),
            vec3(-S2, C2, 0),
            vec3(0, 0, 1)
        );
        mat3 Y3 = mat3(
            vec3(C3, 0, -S3),
            vec3(0, 1, 0),
            vec3(S3, 0, C3)
        );

        *this = Y3 * Z2*X1;

        return *this;
    }

    scalar mat3::yaw() {
        return -atan2(_11, _31)*r2d;
    }

    scalar mat3::pitch() {
        return asin(_21)*r2d;
    }

    scalar mat3::roll() {
        return -atan2(_23, _22)*r2d;
    }

    mat3 mat3::getTranspose() {
        return mat3(row1(), row2(), row3());
    }


    mat3 createYawPitchRollMatrix(scalar yaw, scalar pitch, scalar roll) {
        // 2-3-1 Body-Fixed Euler-Rotation
        scalar C1 = cos(roll*d2r);  //x Roll
        scalar S1 = sin(roll*d2r);
        scalar C2 = cos(pitch*d2r); //z Pitch
        scalar S2 = sin(pitch*d2r);
        scalar C3 = cos(yaw*d2r);   //y Yaw
        scalar S3 = sin(yaw*d2r);

        mat3 X1 = mat3(
            vec3(1, 0, 0),
            vec3(0, C1, S1),
            vec3(0, -S1, C1)
        );
        mat3 Z2 = mat3(
            vec3(C2, S2, 0),
            vec3(-S2, C2, 0),
            vec3(0, 0, 1)
        );
        mat3 Y3 = mat3(
            vec3(C3, 0, -S3),
            vec3(0, 1, 0),
            vec3(S3, 0, C3)
        );

        mat3 m = Y3 * Z2*X1;

        return m;
    }

    mat4 createInverseTransform(vec3 Position, vec3 YawPitchRoll, vec3 Scale) {
        mat4 out = mat4(createYawPitchRollMatrix(YawPitchRoll.x, YawPitchRoll.y, YawPitchRoll.z).getTranspose()) *
            mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0),
                vec4(-Position.x / Scale.x, -Position.y / Scale.y, -Position.z / Scale.z, 1));

        return out;
    }

    vec3 vec4::XYZ() {
        return vec3(x, y, z);
    }

    mat4 invertViewMatrix(const mat4& t) {
        // decompose view matrix into rotation and transform matrix
        mat3 rotation(
            vec3(t._11, t._21, t._31), 
            vec3(t._12, t._22, t._32),
            vec3(t._13, t._23, t._33));
        rotation = rotation.getTranspose();

        vec3 invTransform = vec3(-t._14, -t._24, -t._34);

        mat4 T;
        T.translate(invTransform);
        return mat4(rotation) * T;
    }

    // Pos, Yaw, Pitch, Roll, Forward, Right, Up
    std::tuple<vec3, scalar, scalar, scalar, vec3, vec3, vec3> Decompose(const mat4 & t) {
        vec3 position = vec3(t._14, t._24, t._34);

        mat3 rotation(
            vec3(t._11, t._21, t._31),
            vec3(t._12, t._22, t._32),
            vec3(t._13, t._23, t._33));
        float yaw = rotation.yaw();
        float pitch = rotation.pitch();
        float roll = rotation.roll();

        vec3 forward = -rotation.col3();
        vec3 right = rotation.col1();
        vec3 up = rotation.col2();

        return std::tuple<vec3, scalar, scalar, scalar, vec3, vec3, vec3>(
            position, yaw, pitch, roll, forward, right, up);
    }

    // Forward, Right, Up
    std::tuple<vec3, vec3, vec3> GetUnitVectors(const mat4& t) {
        mat3 rotation(
            vec3(t._11, t._21, t._31),
            vec3(t._12, t._22, t._32),
            vec3(t._13, t._23, t._33));

        vec3 right = rotation.col1();
        vec3 up = rotation.col2();
        vec3 forward = -rotation.col3();

        return std::tuple<vec3, vec3, vec3>(
            forward, right, up);
    }
    */
}

