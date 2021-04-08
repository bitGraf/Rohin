#ifndef GAME_MATH_H
#define GAME_MATH_H

#include "Engine/Core/DataTypes.hpp"
#include "Engine/Core/Base.hpp"

namespace math {
    typedef f32 scalar;

    const scalar pi  = 3.14159265358979323846f;
    const scalar d2r = pi/180.0f;
    const scalar r2d = 180.0f/pi;

    class vec2;
    class vec3;
    class vec4;

    class vec2 {
    public:
        vec2();
        vec2(scalar f);
        vec2(scalar _x, scalar _y);
        vec2(const vec2& v);

        scalar length() const;
        scalar length_2() const;
        vec2 get_unit() const;
        vec2 normalize();

        scalar dot(const vec2& v) const;

        const scalar* ptr() const;
        scalar& operator[](std::size_t idx);
        const scalar& operator[](std::size_t idx) const;

        friend ::std::ostream& operator<<(std::ostream& os, const vec2& v);

        scalar x, y;
    };

    class vec3 {
    public:
        vec3();
        vec3(scalar f);
        vec3(scalar _x, scalar _y, scalar _z);
        vec3(const vec2& v, scalar _z);
        vec3(const vec3& v);

        scalar length() const;
        scalar length_2() const;
        vec3 get_unit() const;
        vec3 normalize();

        scalar dot(const vec3& v) const;
        vec3 cross(const vec3& v) const;
        vec2 asVec2() const;

        const scalar* ptr() const;
        scalar& operator[](std::size_t idx);
        const scalar& operator[](std::size_t idx) const;

        friend ::std::ostream& operator<<(std::ostream& os, const vec3& v);

        scalar x,y,z;
    };

    class vec4 {
    public:
        vec4();
        vec4(scalar f);
        vec4(scalar _x, scalar _y, scalar _z, scalar _w);
        vec4(const vec2& v, scalar _z, scalar _w);
        vec4(const vec3& v, scalar _w);
        vec4(const vec4& v);

        scalar length() const;
        scalar length_2() const;
        vec4 get_unit() const;
        vec4 normalize();

        scalar dot(const vec4& v) const;
        vec2 asVec2() const;
        vec3 asVec3() const;

        const scalar* ptr() const;
        scalar& operator[](std::size_t idx);
        const scalar& operator[](std::size_t idx) const;

        friend ::std::ostream& operator<<(std::ostream& os, const vec4& v);

        scalar x, y, z, w;
    };

    class mat2;
    class mat3;
    class mat4;

    // 2x2 ColumnMajor matrix
    class mat2 {
    public:
        mat2();
        mat2(scalar f);
        mat2(scalar c11, scalar c22);
        mat2(const vec2& col1, const vec2& col2);
        mat2(const mat2& m);

        void identity();
        void transpose();
        mat2 getTranspose() const;

        const scalar* ptr() const;
        vec2& operator[](std::size_t idx);
        const vec2& operator[](std::size_t idx) const;

        vec2 row1() const;
        vec2 row2() const;

        friend ::std::ostream& operator<<(std::ostream& os, const mat2& m);

        vec2 column1, column2;
        //scalar _11, _21, _12, _22;
    };

    // 3x3 ColumnMajor matrix
    class mat3 {
    public:
        mat3();
        mat3(scalar f);
        mat3(scalar c11, scalar c22, scalar c33);
        mat3(const vec3& col1, const vec3& col2, const vec3& col3);
        mat3(const mat3& m);

        void identity();
        void transpose();
        mat3 getTranspose() const;

        const scalar* ptr() const;
        vec3& operator[](std::size_t idx);
        const vec3& operator[](std::size_t idx) const;

        vec3 row1() const;
        vec3 row2() const;
        vec3 row3() const;

        mat2 asMat2() const;

        friend ::std::ostream& operator<<(std::ostream& os, const mat3& m);

        vec3 column1, column2, column3;
        //scalar _11, _21, _31, _12, _22, _32, _13, _23, _33;
    };

    // 4x4 ColumnMajor matrix
    class mat4 {
    public:
        mat4();
        mat4(scalar f);
        mat4(scalar c11, scalar c22, scalar c33, scalar c44);
        mat4(const vec4& col1, const vec4& col2, const vec4& col3, const vec4& col4);
        mat4(const mat4& m);
        // explicitly create mat4 from mat3
        mat4(const mat3& m, scalar f);

        void identity();
        void transpose();
        mat4 getTranspose() const;

        const scalar* ptr() const;
        vec4& operator[](std::size_t idx);
        const vec4& operator[](std::size_t idx) const;

        vec4 row1() const;
        vec4 row2() const;
        vec4 row3() const;
        vec4 row4() const;

        mat2 asMat2() const;
        mat3 asMat3() const;

        friend ::std::ostream& operator<<(std::ostream& os, const mat4& m); ///< Stream operator overload

        vec4 column1, column2, column3, column4;
        //scalar _11, _21, _31, _41, _12, _22, _32, _42, _13, _23, _33, _43, _14, _24, _34, _44;
    };

    // Euler parameter / Quaternion (<x,y,z> imaginary component, w real component)
    class quat {
    public:
        quat(); // create as identy
        quat(scalar _x, scalar _y, scalar _z, scalar _w);
        quat(scalar _x, scalar _y, scalar _z); // reconstruct w (assumes unit q)
        quat(const quat& q);
        quat(const vec3& xyz, scalar _w);

        void reconstructW();
        void reconstructW_Left(); // if the quat was expressed in a left-handed coordinate system

        scalar length_2() const;

        quat normalize();
        quat inv() const;
        vec3 getVectorPart() const;
        scalar dot(const quat& q) const;
        vec4 asVec4() const;

        const scalar* ptr() const;
        scalar& operator[](std::size_t idx);
        const scalar& operator[](std::size_t idx) const;

        friend ::std::ostream& operator<<(std::ostream& os, const quat& q);

        scalar x, y, z, w;
    };

    /* Vector 2 operators */
    bool operator== (const vec2& A, const vec2& B); ///< Vector2 == Vector2
    vec2 operator+ (const vec2& A, const vec2& B);  ///< Vector2 + Vector2
    vec2 operator- (const vec2& A, const vec2& B);  ///< Vector2 - Vector2
    vec2 operator* (const vec2& V, scalar s);       ///< Vector2 X Scalar
    vec2 operator* (scalar s, const vec2& V);       ///< Scalar X Vector2
    vec2 operator/ (const vec2& V, scalar s);       ///< Vector2 / Scalar
    vec2& operator+= (vec2& A, const vec2& B);      ///< Vector2 += Vector2
    vec2& operator-= (vec2& A, const vec2& B);      ///< Vector2 -= Vector2
    vec2& operator*= (vec2& V, scalar s);           ///< Vector2 /= Scalar
    vec2& operator/= (vec2& V, scalar s);           ///< Vector2 /= Scalar
    vec2 operator- (const vec2& V);                 ///< -Vector2

    /* Vector 3 operators*/
    bool operator== (const vec3& A, const vec3& B); ///< Vector3 == Vector3
    vec3 operator+ (const vec3& A, const vec3& B);  ///< Vector3 + Vector3
    vec3 operator- (const vec3& A, const vec3& B);  ///< Vector3 - Vector3
    vec3 operator* (const vec3& V, scalar s);       ///< Vector3 X Scalar
    vec3 operator* (scalar s, const vec3& V);       ///< Scalar X Vector3
    vec3 operator/ (const vec3& V, scalar s);       ///< Vector3 / Scalar
    vec3& operator+= (vec3& A, const vec3& B);      ///< Vector3 += Vector3
    vec3& operator-= (vec3& A, const vec3& B);      ///< Vector3 -= Vector3
    vec3& operator*= (vec3& V, scalar s);           ///< Vector3 /= Scalar
    vec3& operator/= (vec3& V, scalar s);           ///< Vector3 /= Scalar
    vec3 operator- (const vec3& V);                 ///< -Vector3

    /* Vector 4 operators */
    bool operator== (const vec4& A, const vec4& B); ///< Vector4 == Vector4
    vec4 operator+ (const vec4& A, const vec4& B);  ///< Vector4 + Vector4
    vec4 operator- (const vec4& A, const vec4& B);  ///< Vector4 - Vector4
    vec4 operator* (const vec4& V, scalar s);       ///< Vector4 X Scalar
    vec4 operator* (scalar s, const vec4& V);       ///< Scalar X Vector4
    vec4 operator/ (const vec4& V, scalar s);       ///< Vector4 / Scalar
    vec4& operator+= (vec4& A, const vec4& B);      ///< Vector4 += Vector4
    vec4& operator-= (vec4& A, const vec4& B);      ///< Vector4 -= Vector4
    vec4& operator*= (vec4& V, scalar s);           ///< Vector4 /= Scalar
    vec4& operator/= (vec4& V, scalar s);           ///< Vector4 /= Scalar
    vec4 operator- (const vec4& V);                 ///< -Vector4

    /* Matrix 2x2 operators */
    bool operator== (const mat2& A, const mat2& B); ///< Matrix2 == Matrix2
    mat2 operator+ (const mat2& A, const mat2& B);  ///< Matrix2 + Matrix2
    mat2 operator- (const mat2& A, const mat2& B);  ///< Matrix2 - Matrix2
    mat2 operator* (const mat2& M, scalar s);       ///< Matrix3 X Scalar
    mat2 operator* (scalar s, const mat2& M);       ///< Scalar X Matrix2
    mat2 operator* (const mat2& A, const mat2& B);  ///< Matrix2 X Matrix2
    vec2 operator* (const mat2& M, const vec2& V);  ///< Matrix2 X Vector2
    mat2 operator/ (const mat2& M, scalar s);       ///< Matrix2 / Scalar
    mat2& operator+= (mat2& A, const mat2& B);      ///< Matrix2 += Matrix2
    mat2& operator-= (mat2& A, const mat2& B);      ///< Matrix2 -= Matrix2
    mat2& operator*= (mat2& M, scalar s);           ///< Matrix2 X= Scalar
    mat2& operator*= (mat2& A, const mat2& B);      ///< Matrix2 X= Matrix2
    mat2& operator/= (mat2& M, scalar s);           ///< Matrix2 /= Scalar
    mat2 operator- (const mat2& M);                 ///< -Matrix2
    
    /* Matrix 3x3 operators */
    bool operator== (const mat3& A, const mat3& B); ///< Matrix3 == Matrix3
    mat3 operator+ (const mat3& A, const mat3& B);  ///< Matrix3 + Matrix3
    mat3 operator- (const mat3& A, const mat3& B);  ///< Matrix3 - Matrix3
    mat3 operator* (const mat3& M, scalar s);       ///< Matrix3 X Scalar
    mat3 operator* (scalar s, const mat3& M);       ///< Scalar X Matrix3
    mat3 operator* (const mat3& A, const mat3& B);  ///< Matrix3 X Matrix3
    vec3 operator* (const mat3& M, const vec3& V);  ///< Matrix3 X Vector3
    mat3 operator/ (const mat3& M, scalar s);       ///< Matrix3 / Scalar
    mat3& operator+= (mat3& A, const mat3& B);      ///< Matrix3 += Matrix3
    mat3& operator-= (mat3& A, const mat3& B);      ///< Matrix3 -= Matrix3
    mat3& operator*= (mat3& M, scalar s);           ///< Matrix3 X= Scalar
    mat3& operator*= (mat3& A, const mat3& B);      ///< Matrix3 X= Matrix3
    mat3& operator/= (mat3& M, scalar s);           ///< Matrix3 /= Scalar
    mat3 operator- (const mat3& M);                 ///< -Matrix3

    /* Matrix 4x4 operators */
    bool operator== (const mat4& A, const mat4& B); ///< Matrix4 == Matrix4
    mat4 operator+ (const mat4& A, const mat4& B);  ///< Matrix4 + Matrix4
    mat4 operator- (const mat4& A, const mat4& B);  ///< Matrix4 - Matrix4
    mat4 operator* (const mat4& M, scalar s);       ///< Matrix4 X Scalar
    mat4 operator* (scalar s, const mat4& M);       ///< Scalar X Matrix4
    mat4 operator* (const mat4& A, const mat4& B);  ///< Matrix4 X Matrix4
    vec4 operator* (const mat4& M, const vec4& V);  ///< Matrix4 X Vector4
    mat4 operator/ (const mat4& M, scalar s);       ///< Matrix4 / Scalar
    mat4& operator+= (mat4& A, const mat4& B);      ///< Matrix4 += Matrix4
    mat4& operator-= (mat4& A, const mat4& B);      ///< Matrix4 -= Matrix4
    mat4& operator*= (mat4& M, scalar s);           ///< Matrix4 X= Scalar
    mat4& operator*= (mat4& A, const mat4& B);      ///< Matrix4 X= Matrix4
    mat4& operator/= (mat4& M, scalar s);           ///< Matrix4 /= Scalar
    mat4 operator- (const mat4& M);                 ///< -Matrix4

    /* Quaterion operators */
    quat operator* (const quat& q1, const quat& q2);///< Quaternion combination

    /* Utility functions */
    scalar lerp(scalar a, scalar b, scalar f);
    vec2 lerp(const vec2& a, const vec2& b, scalar f);
    vec3 lerp(const vec3& a, const vec3& b, scalar f);
    vec4 lerp(const vec4& a, const vec4& b, scalar f);

    quat lerp(const quat& q1, const quat& q2, scalar f);
    quat slerp(const quat& q1, const quat& q2, scalar f);
    quat fast_slerp(const quat& q1, const quat& q2, scalar f);

    /* Transformation helpers */
    void CreateOrthoProjection(mat4& matrix, scalar left, scalar right, scalar bottom, scalar top, scalar znear, scalar zfar);

    void CreateRotationFromYawPitchRoll(quat& rotation, scalar yaw, scalar pitch, scalar roll);
    void CreateRotationFromYawPitchRoll(mat3& matrix, scalar yaw, scalar pitch, scalar roll);
    void CreateRotationFromYawPitchRoll(mat4& matrix, scalar yaw, scalar pitch, scalar roll);

    void CreateRotationFromYawPitch(quat& rotation, scalar yaw, scalar pitch);
    void CreateRotationFromYawPitch(mat3& matrix, scalar yaw, scalar pitch);
    void CreateRotationFromYawPitch(mat4& matrix, scalar yaw, scalar pitch);

    void CreateRotationFromQuaternion(mat3& matrix, const quat& q);
    void CreateRotationFromQuaternion(mat4& matrix, const quat& q);

    void CreateRotationFromAxisAngle(quat& rotation, const vec3& axis, scalar angle);

    void CreateTranslation(mat4& matrix, const vec3& translation);

    void CreateScale(mat3& matrix, float scaleX, float scaleY, float scaleZ);
    void CreateScale(mat4& matrix, float scaleX, float scaleY, float scaleZ);
    void CreateScale(mat4& matrix, const vec3& scale);

    void CreateTransform(mat4& matrix, const vec3& translation, scalar yaw, scalar pitch);
    void CreateTransform(mat4& matrix, const mat3& rotation, const vec3& translation);
    void CreateTransform(mat4& matrix, const mat3& rotation, const vec3& translation, const vec3& scale);
    void CreateTransform(mat4& matrix, const quat& rotation, const vec3& translation, const vec3& scale);

    void CreateViewFromTransform(mat4& matrix, const mat4& transform);

    void Decompose(const mat4& transform, vec3& translation);
    void Decompose(const mat4& transform, quat& rotation);
    void Decompose(const mat4& transform, scalar& yaw, scalar& pitch);
    void Decompose(const mat4& transform, scalar& yaw, scalar& pitch, scalar& roll);
    void Decompose(const mat4& transform, vec3& Forward, vec3& Right, vec3& Up);

    void Decompose(const mat4& transform, vec3& Translation, mat3& Rotation, vec3& Scale);
    void Decompose(const mat4& transform, vec3& Translation, quat& Rotation, vec3& Scale);

    vec3 TransformPointByQuaternion(const quat& rotation, const vec3& point);
}

#endif
