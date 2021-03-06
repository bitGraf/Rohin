#ifndef GAME_MATH_H
#define GAME_MATH_H

#include "Engine/Core/DataTypes.hpp"
#include "Engine/Core/Base.hpp"

namespace math {
    typedef f32 scalar;

    const scalar pi  = 3.14159265358979323846f;
    const scalar d2r = pi/180.0f;
    const scalar r2d = 180.0f/pi;

    /// 2 Vector
    class vec2 {
    public:
        vec2(); ///< All-zero constructor
        vec2(scalar v); ///< Same-value constructor
        vec2(scalar a, scalar b); ///< Specific-value constructor
        vec2(double* data); ///< Initialize by array
        vec2(int v);

        scalar length(); ///< Find magnitude of vector
        scalar length_2(); ///< Find squared magnitude of vector
        vec2 get_unit(); ///< Get unit vector (doesn't modify)
        vec2 normalize(); ///< Normalize vector and return
        scalar dot(vec2 v); ///< Dot product with another vec2
        scalar cross(vec2 v); ///< Cross product with another vec2

        friend ::std::ostream& operator<<(std::ostream& os, const vec2& v); ///< Stream operator overload

        scalar x, y;
    };

    class vec4;
    /// 3 Vector
    class vec3 {
    public:
        vec3(); ///< All-zero constructor
        vec3(scalar v); ///< Same-value constructor
        vec3(scalar a, scalar b, scalar c); ///< Specific-value constructor
        vec3(double* data); ///< Initialize by array
        vec3(vec2 v, scalar c); ///< Initialize from smaller vector
        vec3(vec4 v); ///< Initialize from larger vector
        vec3(int v);

        scalar length(); ///< Find magnitude of vector
        scalar length_2(); ///< Find squared magnitude of vector
        vec3 get_unit(); ///< Get unit vector (doesn't modify)
        vec3 normalize(); ///< Normalize vector and return
        scalar dot(vec3 v); ///< Dot product with another vec2
        vec3 cross(vec3 v); ///< Cross product with another vec2

        friend ::std::ostream& operator<<(std::ostream& os, const vec3& v); ///< Stream operator overload

        scalar x,y,z;
    };

    /// 4 Vector
    class vec4 {
    public:
        vec4(); ///< All-zero constructor
        vec4(scalar v); ///< Same-value constructor
        vec4(scalar a, scalar b, scalar c, scalar d); ///< Specific-value constructor
        vec4(double* data); ///< Initialize by array
        vec4(vec3 v, scalar d); ///< Initialize from smaller vector
        vec4(int v);

        scalar length(); ///< Find magnitude of vector
        scalar length_2(); ///< Find squared magnitude of vector
        vec4 get_unit(); ///< Get unit vector (doesn't modify)
        vec4 normalize(); ///< Normalize vector and return
        scalar dot(vec4 v); ///< Dot product with another vec2

        vec3 XYZ(); ///< pseudo swizzle

        friend ::std::ostream& operator<<(std::ostream& os, const vec4& v); ///< Stream operator overload

        scalar x, y, z, w;
    };


    /// 2x2 Matrix
    class mat2 {
    public:
        mat2(); ///< Identity constructor
        mat2(scalar v); ///< Same-Value constructor
        mat2(vec2 v1, vec2 v2); ///< Initialize by column vectors
        mat2(scalar c11, scalar c22); ///< Initialize by diagonal values

        vec2 row1() const;  ///< Get row 1
        vec2 row2() const;  ///< Get row 2
        vec2 col1() const;  ///< Get column 1
        vec2 col2() const;  ///< Get column 2

        friend ::std::ostream& operator<<(std::ostream& os, const mat2& v); ///< Stream operator overload

        scalar _11, _21, _12, _22;
    };

    /// 3x3 Matrix
    class mat4;
    class mat3 {
    public:
        mat3(); ///< Identity constructor
        mat3(scalar v); ///< Same-Value constructor
        mat3(vec3 v1, vec3 v2, vec3 v3); ///< Initialize by column vectors
        mat3(scalar c11, scalar c22, scalar c33); ///< Initialize by diagonal values
        mat3(mat4 m);

        vec3 row1() const;  ///< Get row 1
        vec3 row2() const;  ///< Get row 2
        vec3 row3() const;  ///< Get row 3
        vec3 col1() const;  ///< Get column 1
        vec3 col2() const;  ///< Get column 2
        vec3 col3() const;  ///< Get column 3

        /* Make this a 2-3-1 Body-Fixed Euler Rotation Matrix (Degrees) */
        mat3& toYawPitchRoll(scalar yaw, scalar pitch, scalar roll);
        scalar yaw();
        scalar pitch();
        scalar roll();

        mat3 getTranspose();

        friend ::std::ostream& operator<<(std::ostream& os, const mat3& v); ///< Stream operator overload

        scalar _11, _21, _31, _12, _22, _32, _13, _23, _33;
    };

    /// 4x4 Matrix
    class mat4 {
    public:
        mat4();
        mat4(scalar v);
        mat4(vec4 v1, vec4 v2, vec4 v3, vec4 v4);
        mat4(scalar c11, scalar c22, scalar c33, scalar c44);
        mat4(mat3 m);
        mat4(mat3 m, vec3 column, vec3 row, scalar corner);

        vec4 row1() const;  ///< Get row 1
        vec4 row2() const;  ///< Get row 2
        vec4 row3() const;  ///< Get row 3
        vec4 row4() const;  ///< Get row 4
        vec4 col1() const;  ///< Get column 1
        vec4 col2() const;  ///< Get column 2
        vec4 col3() const;  ///< Get column 3
        vec4 col4() const;  ///< Get column 4

        void lookAt(vec3 pos, vec3 targ, vec3 Up);
        void orthoProjection(scalar left, scalar right, scalar bottom, scalar top, scalar znear, scalar zfar);
		void translate(math::vec3 translation_vector);
		void rotate(float angle, math::vec3 rotation_axis);
		void scale(math::vec3 scale_vector);

        friend ::std::ostream& operator<<(std::ostream& os, const mat4& m); ///< Stream operator overload

        scalar _11, _21, _31, _41, _12, _22, _32, _42, _13, _23, _33, _43, _14, _24, _34, _44;
    };

    /* Define mathematical operations */
    bool operator== (const vec2& A, const vec2& B);  ///< Vector2 == Vector2
    bool operator== (const vec3& A, const vec3& B);  ///< Vector3 == Vector3
    bool operator== (const vec4& A, const vec4& B);  ///< Vector4 == Vector4

    bool operator== (const mat2& A, const mat2& B);  ///< Matrix2 == Matrix2
    bool operator== (const mat3& A, const mat3& B);  ///< Matrix3 == Matrix3
    bool operator== (const mat4& A, const mat4& B);  ///< Matrix4 == Matrix4

    vec2 operator+ (const vec2& A, const vec2& B);  ///< Vector2 + Vector2
    vec3 operator+ (const vec3& A, const vec3& B);  ///< Vector3 + Vector3
    vec4 operator+ (const vec4& A, const vec4& B);  ///< Vector4 + Vector4

    mat2 operator+ (const mat2& A, const mat2& B);  ///< Matrix2 + Matrix2
    mat3 operator+ (const mat3& A, const mat3& B);  ///< Matrix3 + Matrix3
    mat4 operator+ (const mat4& A, const mat4& B);  ///< Matrix4 + Matrix4

    vec2 operator- (const vec2& A, const vec2& B);  ///< Vector2 - Vector2
    vec3 operator- (const vec3& A, const vec3& B);  ///< Vector3 - Vector3
    vec4 operator- (const vec4& A, const vec4& B);  ///< Vector4 - Vector4

    mat2 operator- (const mat2& A, const mat2& B);  ///< Matrix2 - Matrix2
    mat3 operator- (const mat3& A, const mat3& B);  ///< Matrix3 - Matrix3
    mat4 operator- (const mat4& A, const mat4& B);  ///< Matrix4 - Matrix4
    
    vec2 operator* (const vec2& V, scalar s);       ///< Vector2 X Scalar
    vec3 operator* (const vec3& V, scalar s);       ///< Vector3 X Scalar
    vec4 operator* (const vec4& V, scalar s);       ///< Vector4 X Scalar

    vec2 operator* (scalar s, const vec2& V);       ///< Scalar X Vector2
    vec3 operator* (scalar s, const vec3& V);       ///< Scalar X Vector3
    vec4 operator* (scalar s, const vec4& V);       ///< Scalar X Vector4

    mat2 operator* (const mat2& M, scalar s);       ///< Matrix3 X Scalar
    mat3 operator* (const mat3& M, scalar s);       ///< Matrix3 X Scalar
    mat4 operator* (const mat4& M, scalar s);       ///< Matrix4 X Scalar

    mat2 operator* (scalar s, const mat2& M);       ///< Scalar X Matrix2
    mat3 operator* (scalar s, const mat3& M);       ///< Scalar X Matrix3
    mat4 operator* (scalar s, const mat4& M);       ///< Scalar X Matrix4

    mat2 operator* (const mat2& A, const mat2& B);  ///< Matrix2 X Matrix2
    mat3 operator* (const mat3& A, const mat3& B);  ///< Matrix3 X Matrix3
    mat4 operator* (const mat4& A, const mat4& B);  ///< Matrix4 X Matrix4

    vec2 operator* (const mat2& M, const vec2& V);  ///< Matrix2 X Vector2
    vec3 operator* (const mat3& M, const vec3& V);  ///< Matrix3 X Vector3
    vec4 operator* (const mat4& M, const vec4& V);  ///< Matrix4 X Vector4

    vec2 operator/ (const vec2& V, scalar s);       ///< Vector2 / Scalar
    vec3 operator/ (const vec3& V, scalar s);       ///< Vector3 / Scalar
    vec4 operator/ (const vec4& V, scalar s);       ///< Vector4 / Scalar

    mat2 operator/ (const mat2& M, scalar s);       ///< Matrix2 / Scalar
    mat3 operator/ (const mat3& M, scalar s);       ///< Matrix3 / Scalar
    mat4 operator/ (const mat4& M, scalar s);       ///< Matrix4 / Scalar

    vec2& operator+= (vec2& A, const vec2& B);      ///< Vector2 += Vector2
    vec3& operator+= (vec3& A, const vec3& B);      ///< Vector3 += Vector3
    vec4& operator+= (vec4& A, const vec4& B);      ///< Vector4 += Vector4

    mat2& operator+= (mat2& A, const mat2& B);      ///< Matrix2 += Matrix2
    mat3& operator+= (mat3& A, const mat3& B);      ///< Matrix3 += Matrix3
    mat4& operator+= (mat4& A, const mat4& B);      ///< Matrix4 += Matrix4

    vec2& operator-= (vec2& A, const vec2& B);      ///< Vector2 -= Vector2
    vec3& operator-= (vec3& A, const vec3& B);      ///< Vector3 -= Vector3
    vec4& operator-= (vec4& A, const vec4& B);      ///< Vector4 -= Vector4

    mat2& operator-= (mat2& A, const mat2& B);      ///< Matrix2 -= Matrix2
    mat3& operator-= (mat3& A, const mat3& B);      ///< Matrix3 -= Matrix3
    mat4& operator-= (mat4& A, const mat4& B);      ///< Matrix4 -= Matrix4

    vec2& operator*= (vec2& V, scalar s);           ///< Vector2 /= Scalar
    vec3& operator*= (vec3& V, scalar s);           ///< Vector3 /= Scalar
    vec4& operator*= (vec4& V, scalar s);           ///< Vector4 /= Scalar

    mat2& operator*= (mat2& M, scalar s);           ///< Matrix2 X= Scalar
    mat3& operator*= (mat3& M, scalar s);           ///< Matrix3 X= Scalar
    mat4& operator*= (mat4& M, scalar s);           ///< Matrix4 X= Scalar

    mat2& operator*= (mat2& A, const mat2& B);      ///< Matrix2 X= Matrix2
    mat3& operator*= (mat3& A, const mat3& B);      ///< Matrix3 X= Matrix3
    mat4& operator*= (mat4& A, const mat4& B);      ///< Matrix4 X= Matrix4

    vec2& operator/= (vec2& V, scalar s);           ///< Vector2 /= Scalar
    vec3& operator/= (vec3& V, scalar s);           ///< Vector3 /= Scalar
    vec4& operator/= (vec4& V, scalar s);           ///< Vector4 /= Scalar

    mat2& operator/= (mat2& M, scalar s);           ///< Matrix2 /= Scalar
    mat3& operator/= (mat3& M, scalar s);           ///< Matrix3 /= Scalar
    mat4& operator/= (mat4& M, scalar s);           ///< Matrix4 /= Scalar

    vec2 operator- (const vec2& V);                 ///< -Vector2
    vec3 operator- (const vec3& V);                 ///< -Vector3
    vec4 operator- (const vec4& V);                 ///< -Vector4

    mat2 operator- (const mat2& M);                 ///< -Matrix2
    mat3 operator- (const mat3& M);                 ///< -Matrix3
    mat4 operator- (const mat4& M);                 ///< -Matrix4

    mat3 createYawPitchRollMatrix(scalar yaw, scalar pitch, scalar roll);
    mat4 createInverseTransform(vec3 Position, vec3 YawPitchRoll, vec3 Scale = vec3(1));

    mat4 invertViewMatrix(const mat4& transform);

    scalar lerp(scalar a, scalar b, scalar f);

    // Pos, Yaw, Pitch, Roll, Forward, Right, Up
    std::tuple<vec3, scalar, scalar, scalar, vec3, vec3, vec3> Decompose(const mat4& transform);

    // Forward, Right, Up
    std::tuple<vec3, vec3, vec3> GetUnitVectors(const mat4& transform);
}

#endif
