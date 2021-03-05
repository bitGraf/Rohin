#include "enpch.hpp"
#include "GameMath.hpp"

namespace math {
    /* vec2 Constructors */
    vec2::vec2() : x(0), y(0) {}
    vec2::vec2(scalar v) : x(v), y(v) {}
    vec2::vec2(scalar a, scalar b) : x(a), y(b) {}
    vec2::vec2(double* data) : x(data[0]), y(data[1]) {}
    vec2::vec2(picojson::array& arr) {
        if (arr.size() > 0)
            x = arr[0].get<double>();
        if (arr.size() > 1)
            y = arr[1].get<double>();
    }
    vec2::vec2(int v) : x(v), y(v) {}

    /* vec3 constructors */
    vec3::vec3() : x(0), y(0), z(0) {}
    vec3::vec3(scalar v) : x(v), y(v), z(v) {}
    vec3::vec3(scalar a, scalar b, scalar c) : x(a), y(b), z(c) {}
    vec3::vec3(double* data) : x(data[0]), y(data[1]), z(data[2]) {}
    vec3::vec3(vec2 v, scalar c) : x(v.x), y(v.y), z(c) {}
    vec3::vec3(vec4 v) : x(v.x), y(v.y), z(v.z) {}
    vec3::vec3(int v) : x(v), y(v), z(v) {}
    vec3::vec3(picojson::array& arr) {
        if (arr.size() > 0)
            x = arr[0].get<double>();
        if (arr.size() > 1)
            y = arr[1].get<double>();
        if (arr.size() > 2)
            z = arr[2].get<double>();
    }

    /* vec4 constructors */
    vec4::vec4() : x(0), y(0), z(0), w(0) {}
    vec4::vec4(scalar v) : x(v), y(v), z(v), w(v) {}
    vec4::vec4(scalar a, scalar b, scalar c, scalar d) : x(a), y(b), z(c), w(d) {}
    vec4::vec4(double* data) : x(data[0]), y(data[1]), z(data[2]), w(data[3]) {}
    vec4::vec4(vec3 v, scalar d) : x(v.x), y(v.y), z(v.z), w(d) {}
    vec4::vec4(int v) : x(v), y(v), z(v), w(v) {}
    vec4::vec4(picojson::array& arr) {
        if (arr.size() > 0)
            x = arr[0].get<double>();
        if (arr.size() > 1)
            y = arr[1].get<double>();
        if (arr.size() > 2)
            z = arr[2].get<double>();
        if (arr.size() > 3)
            w = arr[3].get<double>();
    }

    /* length operator */
    scalar vec2::length() { return sqrt(x*x + y * y); }
    scalar vec3::length() { return sqrt(x*x + y * y + z * z); }
    scalar vec4::length() { return sqrt(x*x + y * y + z * z + w * w); }

    /* length_2 operator */
    scalar vec2::length_2() { return (x*x + y * y); }
    scalar vec3::length_2() { return (x*x + y * y + z * z); }
    scalar vec4::length_2() { return (x*x + y * y + z * z + w * w); }

    /* get_unit */
    vec2 vec2::get_unit() {
        scalar c = 0, l = length();
        if (l > 0.0)
            c = 1 / l;
        return vec2(x*c, y*c);
    }
    vec3 vec3::get_unit() {
        scalar c = 0, l = length();
        if (l > 0.0)
            c = 1 / l;
        return vec3(x*c, y*c, z*c);
    }
    vec4 vec4::get_unit() {
        scalar c = 0, l = length();
        if (l > 0.0)
            c = 1 / l;
        return vec4(x*c, y*c, z*c, w*c);
    }

    /* normalize */
    vec2 vec2::normalize() {
        scalar c = 0, l = length();
        if (l > 0.0)
            c = 1 / l;
        *this = vec2(x*c, y*c);
        return *this;
    }
    vec3 vec3::normalize() {
        scalar c = 0, l = length();
        if (l > 0.0)
            c = 1 / l;
        *this = vec3(x*c, y*c, z*c);
        return *this;
    }
    vec4 vec4::normalize() {
        scalar c = 0, l = length();
        if (l > 0.0)
            c = 1 / l;
        *this = vec4(x*c, y*c, z*c, w*c);
        return *this;
    }

    /* dot */
    scalar vec2::dot(vec2 v) { return (x*v.x + y * v.y); }
    scalar vec3::dot(vec3 v) { return (x*v.x + y * v.y + z * v.z); }
    scalar vec4::dot(vec4 v) { return (x*v.x + y * v.y + z * v.z + w * v.w); }

    /* cross */
    scalar vec2::cross(vec2 v) { return (x*v.y - y * v.x); }
    vec3 vec3::cross(vec3 v) { return vec3(y*v.z - z * v.y, z*v.x - x * v.z, x*v.y - y * v.x); }


    /* mat2 constructors */
    mat2::mat2() :
        _11(1), _12(0),
        _21(0), _22(1)
    {
    }

    mat2::mat2(scalar v) :
        _11(v), _12(v),
        _21(v), _22(v)
    {
    }

    mat2::mat2(vec2 v1, vec2 v2) :
        _11(v1.x), _12(v2.x),
        _21(v1.y), _22(v2.y)
    {
    }

    mat2::mat2(scalar c11, scalar c22) :
        _11(c11), _12(0),
        _21(0), _22(c22)
    {
    }

    /* mat3 constructors */
    mat3::mat3() :
        _11(1), _12(0), _13(0),
        _21(0), _22(1), _23(0),
        _31(0), _32(0), _33(1)
    {
    }

    mat3::mat3(scalar v) :
        _11(v), _12(v), _13(v),
        _21(v), _22(v), _23(v),
        _31(v), _32(v), _33(v)
    {
    }

    mat3::mat3(vec3 v1, vec3 v2, vec3 v3) :
        _11(v1.x), _12(v2.x), _13(v3.x),
        _21(v1.y), _22(v2.y), _23(v3.y),
        _31(v1.z), _32(v2.z), _33(v3.z)
    {
    }

    mat3::mat3(scalar c11, scalar c22, scalar c33) :
        _11(c11), _12(0), _13(0),
        _21(0), _22(c22), _23(0),
        _31(0), _32(0), _33(c33)
    {
    }

    mat3::mat3(mat4 m) :
        _11(m._11), _12(m._12), _13(m._13),
        _21(m._21), _22(m._22), _23(m._23),
        _31(m._31), _32(m._32), _33(m._33)
    {
    }

    /* mat4 constructors */
    mat4::mat4() :
        _11(1), _12(0), _13(0), _14(0),
        _21(0), _22(1), _23(0), _24(0),
        _31(0), _32(0), _33(1), _34(0),
        _41(0), _42(0), _43(0), _44(1)
    {
    }

    mat4::mat4(scalar v) :
        _11(v), _12(v), _13(v), _14(v),
        _21(v), _22(v), _23(v), _24(v),
        _31(v), _32(v), _33(v), _34(v),
        _41(v), _42(v), _43(v), _44(v)
    {
    }

    mat4::mat4(vec4 v1, vec4 v2, vec4 v3, vec4 v4) :
        _11(v1.x), _12(v2.x), _13(v3.x), _14(v4.x),
        _21(v1.y), _22(v2.y), _23(v3.y), _24(v4.y),
        _31(v1.z), _32(v2.z), _33(v3.z), _34(v4.z),
        _41(v1.w), _42(v2.w), _43(v3.w), _44(v4.w)
    {
    }

    mat4::mat4(scalar c11, scalar c22, scalar c33, scalar c44) :
        _11(c11), _12(0), _13(0), _14(0),
        _21(0), _22(c22), _23(0), _24(0),
        _31(0), _32(0), _33(c33), _34(0),
        _41(0), _42(0), _43(0), _44(c44)
    {
    }

    mat4::mat4(mat3 m) :
        _11(m._11), _12(m._12), _13(m._13), _14(0),
        _21(m._21), _22(m._22), _23(m._23), _24(0),
        _31(m._31), _32(m._32), _33(m._33), _34(0),
        _41(0), _42(0), _43(0), _44(1)
    {
    }

    mat4::mat4(mat3 m, vec3 column, vec3 row, scalar corner) :
        _11(m._11), _12(m._12), _13(m._13), _14(column.x),
        _21(m._21), _22(m._22), _23(m._23), _24(column.y),
        _31(m._31), _32(m._32), _33(m._33), _34(column.z),
        _41(row.x), _42(row.y), _43(row.z), _44(corner)
    {
    }

    /* Row and Column accessors */
    vec2 mat2::row1() const { return vec2(_11, _12); }
    vec2 mat2::row2() const { return vec2(_21, _22); }

    vec2 mat2::col1() const { return vec2(_11, _21); }
    vec2 mat2::col2() const { return vec2(_12, _22); }


    vec3 mat3::row1() const { return vec3(_11, _12, _13); }
    vec3 mat3::row2() const { return vec3(_21, _22, _23); }
    vec3 mat3::row3() const { return vec3(_31, _32, _33); }

    vec3 mat3::col1() const { return vec3(_11, _21, _31); }
    vec3 mat3::col2() const { return vec3(_12, _22, _32); }
    vec3 mat3::col3() const { return vec3(_13, _23, _33); }


    vec4 mat4::row1() const { return vec4(_11, _12, _13, _14); }
    vec4 mat4::row2() const { return vec4(_21, _22, _23, _24); }
    vec4 mat4::row3() const { return vec4(_31, _32, _33, _34); }
    vec4 mat4::row4() const { return vec4(_41, _42, _43, _44); }

    vec4 mat4::col1() const { return vec4(_11, _21, _31, _41); }
    vec4 mat4::col2() const { return vec4(_12, _22, _32, _42); }
    vec4 mat4::col3() const { return vec4(_13, _23, _33, _43); }
    vec4 mat4::col4() const { return vec4(_14, _24, _34, _44); }


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

    /* Define mathematical operations */
    bool operator== (const vec2& A, const vec2& B) {
        return (A.x == B.x && A.y == B.y);
    }
    bool operator== (const vec3& A, const vec3& B) {
        return (A.x == B.x && A.y == B.y && A.z == B.z);
    }
    bool operator== (const vec4& A, const vec4& B) {
        return (A.x == B.x && A.y == B.y && A.z == B.z && A.w == B.w);
    }

    bool operator== (const mat2& A, const mat2& B) {
        return (A.row1() == B.row1()) &&
               (A.row2() == B.row2());
    }
    bool operator== (const mat3& A, const mat3& B) {
        return (A.row1() == B.row1()) &&
               (A.row2() == B.row2()) &&
               (A.row3() == B.row3());
    }
    bool operator== (const mat4& A, const mat4& B) {
        return (A.row1() == B.row1()) &&
               (A.row2() == B.row2()) &&
               (A.row3() == B.row3()) &&
               (A.row4() == B.row4());
    }

    vec2 operator+ (const vec2& A, const vec2& B) {
        return vec2(A.x + B.x, A.y + B.y);
    }
    vec3 operator+ (const vec3& A, const vec3& B) {
        return vec3(A.x + B.x, A.y + B.y, A.z + B.z);
    }
    vec4 operator+ (const vec4& A, const vec4& B) {
        return vec4(A.x + B.x, A.y + B.y, A.z + B.z, A.w + B.w);
    }

    mat2 operator+ (const mat2& A, const mat2& B) {
        return mat2(A.col1() + B.col1(), A.col2() + B.col2());
    }
    mat3 operator+ (const mat3& A, const mat3& B) {
        return mat3(A.col1() + B.col1(), A.col2() + B.col2(), A.col3() + B.col3());
    }
    mat4 operator+ (const mat4& A, const mat4& B) {
        return mat4(A.col1() + B.col1(), A.col2() + B.col2(), A.col3() + B.col3(), A.col4() + B.col4());
    }

    vec2 operator- (const vec2& A, const vec2& B) {
        return vec2(A.x - B.x, A.y - B.y);
    }
    vec3 operator- (const vec3& A, const vec3& B) {
        return vec3(A.x - B.x, A.y - B.y, A.z - B.z);
    }
    vec4 operator- (const vec4& A, const vec4& B) {
        return vec4(A.x - B.x, A.y - B.y, A.z - B.z, A.w - B.w);
    }

    mat2 operator- (const mat2& A, const mat2& B) {
        return mat2(A.col1() - B.col1(), A.col2() - B.col2());
    }
    mat3 operator- (const mat3& A, const mat3& B) {
        return mat3(A.col1() - B.col1(), A.col2() - B.col2(), A.col3() - B.col3());
    }
    mat4 operator- (const mat4& A, const mat4& B) {
        return mat4(A.col1() - B.col1(), A.col2() - B.col2(), A.col3() - B.col3(), A.col4() - B.col4());
    }

    vec2 operator* (const vec2& V, scalar s) {
        return vec2(V.x*s, V.y*s);
    }
    vec3 operator* (const vec3& V, scalar s) {
        return vec3(V.x*s, V.y*s, V.z*s);
    }
    vec4 operator* (const vec4& V, scalar s) {
        return vec4(V.x*s, V.y*s, V.z*s, V.w*s);
    }

    vec2 operator* (scalar s, const vec2& V) {
        return vec2(V.x*s, V.y*s);
    }
    vec3 operator* (scalar s, const vec3& V) {
        return vec3(V.x*s, V.y*s, V.z*s);
    }
    vec4 operator* (scalar s, const vec4& V) {
        return vec4(V.x*s, V.y*s, V.z*s, V.w*s);
    }

    mat2 operator* (const mat2& M, scalar s) {
        return mat2(M.col1()*s, M.col2()*s);
    }
    mat3 operator* (const mat3& M, scalar s) {
        return mat3(M.col1()*s, M.col2()*s, M.col3()*s);
    }
    mat4 operator* (const mat4& M, scalar s) {
        return mat4(M.col1()*s, M.col2()*s, M.col3()*s, M.col4()*s);
    }

    mat2 operator* (scalar s, const mat2& M) {
        return mat2(M.col1()*s, M.col2()*s);
    }
    mat3 operator* (scalar s, const mat3& M) {
        return mat3(M.col1()*s, M.col2()*s, M.col3()*s);
    }
    mat4 operator* (scalar s, const mat4& M) {
        return mat4(M.col1()*s, M.col2()*s, M.col3()*s, M.col4()*s);
    }

    mat2 operator* (const mat2& A, const mat2& B) {
        return mat2(
            vec2(
                A.row1().dot(B.col1()),
                A.row2().dot(B.col1())),
            vec2(
                A.row1().dot(B.col2()),
                A.row2().dot(B.col2()))
        );
    }
    mat3 operator* (const mat3& A, const mat3& B) {
        return mat3(
            vec3(
                A.row1().dot(B.col1()),
                A.row2().dot(B.col1()),
                A.row3().dot(B.col1())),
            vec3(
                A.row1().dot(B.col2()),
                A.row2().dot(B.col2()),
                A.row3().dot(B.col2())),
            vec3(
                A.row1().dot(B.col3()),
                A.row2().dot(B.col3()),
                A.row3().dot(B.col3()))
        );
    }
    mat4 operator* (const mat4& A, const mat4& B) {
        return mat4(
            vec4(
                A.row1().dot(B.col1()),
                A.row2().dot(B.col1()),
                A.row3().dot(B.col1()),
                A.row4().dot(B.col1())),
            vec4(
                A.row1().dot(B.col2()),
                A.row2().dot(B.col2()),
                A.row3().dot(B.col2()),
                A.row4().dot(B.col2())),
            vec4(
                A.row1().dot(B.col3()),
                A.row2().dot(B.col3()),
                A.row3().dot(B.col3()),
                A.row4().dot(B.col3())),
            vec4(
                A.row1().dot(B.col4()),
                A.row2().dot(B.col4()),
                A.row3().dot(B.col4()),
                A.row4().dot(B.col4()))
        );
    }

    vec2 operator* (const mat2& M, const vec2& V) {
        return vec2(
            M.row1().dot(V),
            M.row2().dot(V)
        );
    }
    vec3 operator* (const mat3& M, const vec3& V) {
        return vec3(
            M.row1().dot(V),
            M.row2().dot(V),
            M.row3().dot(V)
        );
    }
    vec4 operator* (const mat4& M, const vec4& V) {
        return vec4(
            M.row1().dot(V),
            M.row2().dot(V),
            M.row3().dot(V),
            M.row4().dot(V)
        );
    }

    vec2 operator/ (const vec2& V, scalar s) {
        return vec2(V.x / s, V.y / s);
    }
    vec3 operator/ (const vec3& V, scalar s) {
        return vec3(V.x / s, V.y / s, V.z / s);
    }
    vec4 operator/ (const vec4& V, scalar s) {
        return vec4(V.x / s, V.y / s, V.z / s, V.w / s);
    }

    mat2 operator/ (const mat2& M, scalar s) {
        return mat2(M.col1() / s, M.col2() / s);
    }
    mat3 operator/ (const mat3& M, scalar s) {
        return mat3(M.col1() / s, M.col2() / s, M.col3() / s);
    }
    mat4 operator/ (const mat4& M, scalar s) {
        return mat4(M.col1() / s, M.col2() / s, M.col3() / s, M.col4() / s);
    }

    vec2& operator+= (vec2& A, const vec2& B) {
        A = vec2(A.x + B.x, A.y + B.y);
        return A;
    }
    vec3& operator+= (vec3& A, const vec3& B) {
        A = vec3(A.x + B.x, A.y + B.y, A.z + B.z);
        return A;
    }
    vec4& operator+= (vec4& A, const vec4& B) {
        A = vec4(A.x + B.x, A.y + B.y, A.z + B.z, A.w + B.w);
        return A;
    }

    mat2& operator+= (mat2& A, const mat2& B) {
        A = mat2(A.col1() + B.col1(), A.col2() + B.col2());
        return A;
    }
    mat3& operator+= (mat3& A, const mat3& B) {
        A = mat3(A.col1() + B.col1(), A.col2() + B.col2(), A.col3() + B.col3());
        return A;
    }
    mat4& operator+= (mat4& A, const mat4& B) {
        A = mat4(A.col1() + B.col1(), A.col2() + B.col2(), A.col3() + B.col3(), A.col4() + B.col4());
        return A;
    }

    vec2& operator-= (vec2& A, const vec2& B) {
        A = vec2(A.x - B.x, A.y - B.y);
        return A;
    }
    vec3& operator-= (vec3& A, const vec3& B) {
        A = vec3(A.x - B.x, A.y - B.y, A.z - B.z);
        return A;
    }
    vec4& operator-= (vec4& A, const vec4& B) {
        A = vec4(A.x - B.x, A.y - B.y, A.z - B.z, A.w - B.w);
        return A;
    }

    mat2& operator-= (mat2& A, const mat2& B) {
        A = mat2(A.col1() - B.col1(), A.col2() - B.col2());
        return A;
    }
    mat3& operator-= (mat3& A, const mat3& B) {
        A = mat3(A.col1() - B.col1(), A.col2() - B.col2(), A.col3() - B.col3());
        return A;
    }
    mat4& operator-= (mat4& A, const mat4& B) {
        A = mat4(A.col1() - B.col1(), A.col2() - B.col2(), A.col3() - B.col3(), A.col4() - B.col4());
        return A;
    }

    vec2& operator*= (vec2& V, scalar s) {
        V = vec2(V.x*s, V.y*s);
        return V;
    }
    vec3& operator*= (vec3& V, scalar s) {
        V = vec3(V.x*s, V.y*s, V.z*s);
        return V;
    }
    vec4& operator*= (vec4& V, scalar s) {
        V = vec4(V.x*s, V.y*s, V.z*s, V.w*s);
        return V;
    }

    mat2& operator*= (mat2& M, scalar s) {
        M = mat2(M.col1()*s, M.col2()*s);
        return M;
    }
    mat3& operator*= (mat3& M, scalar s) {
        M = mat3(M.col1()*s, M.col2()*s, M.col3()*s);
        return M;
    }
    mat4& operator*= (mat4& M, scalar s) {
        M = mat4(M.col1()*s, M.col2()*s, M.col3()*s, M.col4()*s);
        return M;
    }

    mat2& operator*= (mat2& A, const mat2& B) {
        A = mat2(
            vec2(
                A.row1().dot(B.col1()),
                A.row2().dot(B.col1())),
            vec2(
                A.row1().dot(B.col2()),
                A.row2().dot(B.col2()))
        );
        return A;
    }
    mat3& operator*= (mat3& A, const mat3& B) {
        A = mat3(
            vec3(
                A.row1().dot(B.col1()),
                A.row2().dot(B.col1()),
                A.row3().dot(B.col1())),
            vec3(
                A.row1().dot(B.col2()),
                A.row2().dot(B.col2()),
                A.row3().dot(B.col2())),
            vec3(
                A.row1().dot(B.col3()),
                A.row2().dot(B.col3()),
                A.row3().dot(B.col3()))
        );
        return A;
    }
    mat4& operator*= (mat4& A, const mat4& B) {
        A = mat4(
            vec4(
                A.row1().dot(B.col1()),
                A.row2().dot(B.col1()),
                A.row3().dot(B.col1()),
                A.row4().dot(B.col1())),
            vec4(
                A.row1().dot(B.col2()),
                A.row2().dot(B.col2()),
                A.row3().dot(B.col2()),
                A.row4().dot(B.col2())),
            vec4(
                A.row1().dot(B.col3()),
                A.row2().dot(B.col3()),
                A.row3().dot(B.col3()),
                A.row4().dot(B.col3())),
            vec4(
                A.row1().dot(B.col4()),
                A.row2().dot(B.col4()),
                A.row3().dot(B.col4()),
                A.row4().dot(B.col4()))
        );
        return A;
    }

    vec2& operator/= (vec2& V, scalar s) {
        V = vec2(V.x / s, V.y / s);
        return V;
    }
    vec3& operator/= (vec3& V, scalar s) {
        V = vec3(V.x / s, V.y / s, V.z / s);
        return V;
    }
    vec4& operator/= (vec4& V, scalar s) {
        V = vec4(V.x / s, V.y / s, V.z / s, V.w / s);
        return V;
    }

    mat2& operator/= (mat2& M, scalar s) {
        M = mat2(M.col1() / s, M.col2() / s);
        return M;
    }
    mat3& operator/= (mat3& M, scalar s) {
        M = mat3(M.col1() / s, M.col2() / s, M.col3() / s);
        return M;
    }
    mat4& operator/= (mat4& M, scalar s) {
        M = mat4(M.col1() / s, M.col2() / s, M.col3() / s, M.col4() / s);
        return M;
    }

    vec2 operator- (const vec2& V) {
        return vec2(-V.x, -V.y);
    }
    vec3 operator- (const vec3& V) {
        return vec3(-V.x, -V.y, -V.z);
    }
    vec4 operator- (const vec4& V) {
        return vec4(-V.x, -V.y, -V.z, -V.w);
    }

    mat2 operator- (const mat2& M) {
        return mat2(-M.col1(), -M.col2());
    }
    mat3 operator- (const mat3& M) {
        return mat3(-M.col1(), -M.col2(), -M.col3());
    }
    mat4 operator- (const mat4& M) {
        return mat4(-M.col1(), -M.col2(), -M.col3(), -M.col4());
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

    scalar lerp(scalar a, scalar b, scalar f) {
        return a + f * (b - a);
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

        vec3 forward = -rotation.col3();
        vec3 right = rotation.col1();
        vec3 up = rotation.col2();

        return std::tuple<vec3, vec3, vec3>(
            forward, right, up);
    }


    /* ostream function overloads */
    ::std::ostream & operator<<(std::ostream & os, const vec2 & v)
    {
        // TODO: float formatting
        os << "(" << v.x << ", " << v.y << ") ";
        return os;
    }

    ::std::ostream & operator<<(std::ostream & os, const vec3 & v)
    {
        // TODO: float formatting
        os << "(" << v.x << ", " << v.y << ", " << v.z << ") ";
        return os;
    }

    ::std::ostream & operator<<(std::ostream & os, const vec4 & v)
    {
        // TODO: float formatting
        os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ") ";
        return os;
    }

    std::ostream & operator<<(std::ostream & os, const mat2 & m)
    {
        // TODO: float formatting
        // [1,2;3,4]
        os << '[';
        os << m._11 << ',' << m._12 << ';';
        os << m._21 << ',' << m._22 << ']';

        return os;
    }

    std::ostream & operator<<(std::ostream & os, const mat3 & m)
    {
        // TODO: float formatting
        // [1,2,3;4,5,6;7,8,9]
        os << '[';
        os << m._11 << ',' << m._12 << ',' << m._13 << ';';
        os << m._21 << ',' << m._22 << ',' << m._23 << ';';
        os << m._31 << ',' << m._32 << ',' << m._33 << ']';

        return os;
    }

    std::ostream & operator<<(std::ostream & os, const mat4 & m)
    {
        // TODO: float formatting
        // [1,2,3,4;4,5,6,7;9,10,10,12;13,14,15,16]
        os << '[';
        os << m._11 << ',' << m._12 << ',' << m._13 << ',' << m._14 << ';';
        os << m._21 << ',' << m._22 << ',' << m._23 << ',' << m._24 << ';';
        os << m._31 << ',' << m._32 << ',' << m._33 << ',' << m._34 << ';';
        os << m._41 << ',' << m._42 << ',' << m._43 << ',' << m._44 << ']';
        
        return os;
    }
}

