#include <enpch.hpp>

#include "Engine/Core/GameMath.hpp"

void TestVecConstructors() {
    using namespace math;

    std::cout << "==============================================" << std::endl;
    std::cout << "Testing Vector Constructors" << std::endl;

    std::cout << "vec2(): " << vec2() << std::endl;
    std::cout << "vec2(): " << vec2(1.25f) << std::endl;
    std::cout << "vec2(): " << vec2(2.35, 4.0f) << std::endl;
    std::cout << "vec2(): " << vec2(vec2(.5f, -.1f)) << std::endl;

    std::cout << "vec3(): " << vec3() << std::endl;
    std::cout << "vec3(): " << vec3(1.25f) << std::endl;
    std::cout << "vec3(): " << vec3(2.35, 4.0f, 6.1f) << std::endl;
    std::cout << "vec3(): " << vec3(vec3(.5f, -.1f, 0.5f)) << std::endl;
    std::cout << "vec3(): " << vec3(vec2(.5f, -.1f), 0.5f) << std::endl;

    std::cout << "vec4(): " << vec4() << std::endl;
    std::cout << "vec4(): " << vec4(1.25f) << std::endl;
    std::cout << "vec4(): " << vec4(2.35, 4.0f, 6.1f) << std::endl;
    std::cout << "vec4(): " << vec4(vec4(.5f, -.1f, 0.5f, -4.0f)) << std::endl;
    std::cout << "vec4(): " << vec4(vec3(.5f, -.1f, 4.0f), 0.5f) << std::endl;
    std::cout << "==============================================" << std::endl;
}

void TestSimpleRotations() {
    using namespace math;

    std::cout << "==============================================" << std::endl;
    mat4 rotation;
    vec3 forward, up, right;

    math::Decompose(rotation, forward, right, up);
    std::cout << "Identity: " << rotation << std::endl;
    std::cout << "Forward: " << forward << std::endl;
    std::cout << "Right:   " << right << std::endl;
    std::cout << "Up:      " << up << std::endl;

    math::CreateRotationFromYawPitchRoll(rotation, 0, 0, 0);
    math::Decompose(rotation, forward, right, up);
    std::cout << "Identity: " << rotation << std::endl;
    std::cout << "Forward: " << forward << std::endl;
    std::cout << "Right:   " << right << std::endl;
    std::cout << "Up:      " << up << std::endl;

    math::CreateRotationFromYawPitchRoll(rotation, 90, 0, 0);
    math::Decompose(rotation, forward, right, up);
    std::cout << "90 deg yaw: " << rotation << std::endl;
    std::cout << "Forward: " << forward << std::endl;
    std::cout << "Right:   " << right << std::endl;
    std::cout << "Up:      " << up << std::endl;

    math::CreateRotationFromYawPitchRoll(rotation, 0, 90, 0);
    math::Decompose(rotation, forward, right, up);
    std::cout << "90 deg pitch: " << rotation << std::endl;
    std::cout << "Forward: " << forward << std::endl;
    std::cout << "Right:   " << right << std::endl;
    std::cout << "Up:      " << up << std::endl;

    math::CreateRotationFromYawPitchRoll(rotation, 0, 0, 90);
    math::Decompose(rotation, forward, right, up);
    std::cout << "90 deg roll: " << rotation << std::endl;
    std::cout << "Forward: " << forward << std::endl;
    std::cout << "Right:   " << right << std::endl;
    std::cout << "Up:      " << up << std::endl;
    std::cout << "==============================================" << std::endl;
}

void TestTranspose() {
    using namespace math;

    std::cout << "==============================================" << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "Transpose Test: " << std::endl;
    mat4 m = mat4(vec4(1, 5, 9, 13), vec4(2, 6, 10, 14), vec4(3, 7, 11, 15), vec4(4, 8, 12, 16));
    printf("|%3.0f %3.0f %3.0f %3.0f|\n", m.column1[0], m.column2[0], m.column3[0], m.column4[0]);
    printf("|%3.0f %3.0f %3.0f %3.0f|\n", m.column1[1], m.column2[1], m.column3[1], m.column4[1]);
    printf("|%3.0f %3.0f %3.0f %3.0f|\n", m.column1[2], m.column2[2], m.column3[2], m.column4[2]);
    printf("|%3.0f %3.0f %3.0f %3.0f|\n", m.column1[3], m.column2[3], m.column3[3], m.column4[3]);
    printf("\n");
    m.transpose();
    printf("|%3.0f %3.0f %3.0f %3.0f|\n", m.column1[0], m.column2[0], m.column3[0], m.column4[0]);
    printf("|%3.0f %3.0f %3.0f %3.0f|\n", m.column1[1], m.column2[1], m.column3[1], m.column4[1]);
    printf("|%3.0f %3.0f %3.0f %3.0f|\n", m.column1[2], m.column2[2], m.column3[2], m.column4[2]);
    printf("|%3.0f %3.0f %3.0f %3.0f|\n", m.column1[3], m.column2[3], m.column3[3], m.column4[3]);
    printf("\n");
    std::cout << "==============================================" << std::endl;
    mat3 m2 = m.asMat3();
    printf("|%3.0f %3.0f %3.0f|\n", m2.column1[0], m2.column2[0], m2.column3[0]);
    printf("|%3.0f %3.0f %3.0f|\n", m2.column1[1], m2.column2[1], m2.column3[1]);
    printf("|%3.0f %3.0f %3.0f|\n", m2.column1[2], m2.column2[2], m2.column3[2]);
    printf("\n");
    m2.transpose();
    printf("|%3.0f %3.0f %3.0f|\n", m2.column1[0], m2.column2[0], m2.column3[0]);
    printf("|%3.0f %3.0f %3.0f|\n", m2.column1[1], m2.column2[1], m2.column3[1]);
    printf("|%3.0f %3.0f %3.0f|\n", m2.column1[2], m2.column2[2], m2.column3[2]);
    printf("\n");
    std::cout << "==============================================" << std::endl;
    mat2 m3 = m2.asMat2();
    printf("|%3.0f %3.0f|\n", m3.column1[0], m3.column2[0]);
    printf("|%3.0f %3.0f|\n", m3.column1[1], m3.column2[1]);
    printf("\n");
    m3.transpose();
    printf("|%3.0f %3.0f|\n", m3.column1[0], m3.column2[0]);
    printf("|%3.0f %3.0f|\n", m3.column1[1], m3.column2[1]);
    printf("\n");
    std::cout << "==============================================" << std::endl;
    std::cout << "==============================================" << std::endl;
}

void TestViewMatrixTransformation() {
    using namespace math;

    mat4 transform;
    math::CreateTransform(transform, vec3(2, 3, 5), 30, 75);
    vec3 position;
    scalar yaw, pitch;
    math::Decompose(transform, yaw, pitch);
    math::Decompose(transform, position);
    vec3 vert(5.0f, -2.0f, 7.5f);
    vec4 v(vert, 1.0f);

    std::cout << "Vertex:    " << vert << std::endl;
    std::cout << "Transform: " << transform << std::endl;
    std::cout << "Position:  " << position << std::endl;
    std::cout << "Yaw/Pitch: " << yaw << "/" << pitch << std::endl;
    std::cout << "Vertex:    " << (transform * v).asVec3() << std::endl;
    mat4 viewMatrix;
    math::CreateViewFromTransform(viewMatrix, transform);
    std::cout << "Vertex:    " << (viewMatrix * transform * v).asVec3() << std::endl;
}

void TestCreateFromAxisAngle() {
    using namespace math;

    quat rotation;
    vec3 pos = vec3(1, 2, 3);
    std::cout << "position: " << pos << std::endl;
    math::CreateRotationFromAxisAngle(rotation, vec3(1, 0, 0), 90.0f);
    //std::cout << "Rotate 90 X: " << rotation << std::endl;
    pos = math::TransformPointByQuaternion(rotation, pos);
    std::cout << "new position: " << pos << std::endl;
    math::CreateRotationFromAxisAngle(rotation, vec3(0, 1, 0), 90.0f);
    //std::cout << "Rotate 90 Y: " << rotation << std::endl;
    pos = math::TransformPointByQuaternion(rotation, pos);
    std::cout << "new position: " << pos << std::endl;
    math::CreateRotationFromAxisAngle(rotation, vec3(0, 0, 1), 90.0f);
    //std::cout << "Rotate 90 Z: " << rotation << std::endl;
    pos = math::TransformPointByQuaternion(rotation, pos);
    std::cout << "new position: " << pos << std::endl;
}

void TestQuaternionTransformations() {
    using namespace math;

    quat rotation1, rotation2;
    vec3 pos = vec3(1, 2, 3);
    math::CreateRotationFromAxisAngle(rotation1, vec3(1, 0, 0), 45);
    rotation2 = rotation1 * rotation1;

    std::cout << "Rotated by  0: " << pos << std::endl;
    std::cout << "Rotated by 45: " << math::TransformPointByQuaternion(rotation1, pos) << std::endl;
    std::cout << "Rotated by 90: " << math::TransformPointByQuaternion(rotation2, pos) << std::endl;

    std::cout << "length^2 1: " << rotation1.length_2() << std::endl;
    std::cout << "length^2 2: " << rotation2.length_2() << std::endl;
}

void TestQuaternionCombination() {
    using namespace math;

    quat bindPose, pose, ref;
    math::CreateRotationFromAxisAngle(bindPose, vec3(2, 4, -6.1f), 35.0f);
    math::CreateRotationFromAxisAngle(pose, vec3(2, 4, -6.1f), 65.0f);
    math::CreateRotationFromAxisAngle(ref, vec3(2, 4, -6.1f), 30.0f);

    quat diff = bindPose.inv() * pose;
    std::cout << "Diff: " << diff << std::endl;
    std::cout << "Ref:  " << ref << std::endl;
}