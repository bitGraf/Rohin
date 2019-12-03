#include "GameMath.hpp"

/* vec2 Constructors */
math::vec2::vec2() : x(0), y(0) {}
math::vec2::vec2(math::scalar v) : x(v), y(v) {}
math::vec2::vec2(math::scalar a, math::scalar b) : x(a), y(b) {}
math::vec2::vec2(double* data) : x(data[0]), y(data[1]) {}

/* vec3 constructors */
math::vec3::vec3() : x(0), y(0), z(0) {}
math::vec3::vec3(math::scalar v) : x(v), y(v), z(v) {}
math::vec3::vec3(math::scalar a, math::scalar b, math::scalar c) : x(a), y(b), z(c) {}
math::vec3::vec3(double* data) : x(data[0]), y(data[1]), z(data[2]) {}
math::vec3::vec3(math::vec2 v, math::scalar c) : x(v.x), y(v.y), z(c) {}
math::vec3::vec3(math::vec4 v) : x(v.x), y(v.y), z(v.z) {}

/* vec4 constructors */
math::vec4::vec4() : x(0), y(0), z(0), w(0) {}
math::vec4::vec4(math::scalar v) : x(v), y(v), z(v), w(v) {}
math::vec4::vec4(math::scalar a, math::scalar b, math::scalar c, math::scalar d) : x(a), y(b), z(c), w(d) {}
math::vec4::vec4(double* data) : x(data[0]), y(data[1]), z(data[2]), w(data[3]) {}
math::vec4::vec4(math::vec3 v, math::scalar d) : x(v.x), y(v.y), z(v.z), w(d) {}

/* length operator */
math::scalar math::vec2::length() { return sqrt(x*x + y*y); }
math::scalar math::vec3::length() { return sqrt(x*x + y*y + z*z); }
math::scalar math::vec4::length() { return sqrt(x*x + y*y + z*z + w*w); }

/* length_2 operator */
math::scalar math::vec2::length_2() { return (x*x + y*y); }
math::scalar math::vec3::length_2() { return (x*x + y*y + z*z); }
math::scalar math::vec4::length_2() { return (x*x + y*y + z*z + w*w); }

/* get_unit */
math::vec2 math::vec2::get_unit() { 
    math::scalar c = 0, l = length(); 
    if (l > 0.01)
        c = 1 / l;
    return vec2(x*c, y*c); 
}
math::vec3 math::vec3::get_unit() { 
    math::scalar c = 0, l = length();
    if (l > 0.01)
        c = 1 / l;
    return vec3(x*c, y*c, z*c); 
}
math::vec4 math::vec4::get_unit() { 
    math::scalar c = 0, l = length();
    if (l > 0.01)
        c = 1 / l;
    return vec4(x*c, y*c, z*c, w*c); 
}

/* normalize */
math::vec2 math::vec2::normalize() { 
    scalar c = 0, l = length();
    if (l > 0.01)
        c = 1 / l;
    *this = vec2(x*c, y*c); 
    return *this; 
}
math::vec3 math::vec3::normalize() { 
    scalar c = 0, l = length();
    if (l > 0.01)
        c = 1 / l;
    *this = vec3(x*c, y*c, z*c); 
    return *this; 
}
math::vec4 math::vec4::normalize() { 
    scalar c = 0, l = length();
    if (l > 0.01)
        c = 1 / l;
    *this = vec4(x*c, y*c, z*c, w*c); 
    return *this; 
}

/* dot */
math::scalar math::vec2::dot(math::vec2 v) { return (x*v.x + y*v.y); }
math::scalar math::vec3::dot(math::vec3 v) { return (x*v.x + y*v.y + z*v.z); }
math::scalar math::vec4::dot(math::vec4 v) { return (x*v.x + y*v.y + z*v.z + w*v.w); }

/* cross */
math::scalar math::vec2::cross(math::vec2 v) { return (x*v.y - y*v.x); }
math::vec3 math::vec3::cross(math::vec3 v) { return vec3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y-y*v.x); }


/* mat2 constructors */
math::mat2::mat2() :
    _11(1), _12(0),
    _21(0), _22(1)
{
}

math::mat2::mat2(math::scalar v) :
    _11(v), _12(v),
    _21(v), _22(v)
{
}

math::mat2::mat2(vec2 v1, vec2 v2) :
    _11(v1.x), _12(v2.x),
    _21(v1.y), _22(v2.y)
{
}

math::mat2::mat2(math::scalar c11, math::scalar c22) :
    _11(c11), _12(0),
    _21(0), _22(c22)
{
}

/* mat3 constructors */
math::mat3::mat3() :
    _11(1), _12(0), _13(0),
    _21(0), _22(1), _23(0),
    _31(0), _32(0), _33(1)
{
}

math::mat3::mat3(math::scalar v) :
    _11(v), _12(v), _13(v),
    _21(v), _22(v), _23(v),
    _31(v), _32(v), _33(v)
{
}

math::mat3::mat3(vec3 v1, vec3 v2, vec3 v3) :
    _11(v1.x), _12(v2.x), _13(v3.x),
    _21(v1.y), _22(v2.y), _23(v3.y),
    _31(v1.z), _32(v2.z), _33(v3.z)
{
}

math::mat3::mat3(math::scalar c11, math::scalar c22, math::scalar c33) :
    _11(c11), _12(0),   _13(0),
    _21(0),   _22(c22), _23(0),
    _31(0),   _32(0),   _33(c33)
{
}

math::mat3::mat3(mat4 m) :
    _11(m._11), _12(m._12), _13(m._13),
    _21(m._21), _22(m._22), _23(m._23),
    _31(m._31), _32(m._32), _33(m._33)
{
}

/* mat4 constructors */
math::mat4::mat4() :
    _11(1), _12(0), _13(0), _14(0),
    _21(0), _22(1), _23(0), _24(0),
    _31(0), _32(0), _33(1), _34(0),
    _41(0), _42(0), _43(0), _44(1)
{
}

math::mat4::mat4(math::scalar v) :
    _11(v), _12(v), _13(v), _14(v),
    _21(v), _22(v), _23(v), _24(v),
    _31(v), _32(v), _33(v), _34(v),
    _41(v), _42(v), _43(v), _44(v)
{
}

math::mat4::mat4(vec4 v1, vec4 v2, vec4 v3, vec4 v4) :
    _11(v1.x), _12(v2.x), _13(v3.x), _14(v4.x),
    _21(v1.y), _22(v2.y), _23(v3.y), _24(v4.y),
    _31(v1.z), _32(v2.z), _33(v3.z), _34(v4.z),
    _41(v1.w), _42(v2.w), _43(v3.w), _44(v4.w)
{
}

math::mat4::mat4(math::scalar c11, math::scalar c22, math::scalar c33, math::scalar c44) :
    _11(c11), _12(0),   _13(0),   _14(0),
    _21(0),   _22(c22), _23(0),   _24(0),
    _31(0),   _32(0),   _33(c33), _34(0),
    _41(0),   _42(0),   _43(0),   _44(c44)
{
}

math::mat4::mat4(mat3 m) :
    _11(m._11), _12(m._12), _13(m._13), _14(0),
    _21(m._21), _22(m._22), _23(m._23), _24(0),
    _31(m._31), _32(m._32), _33(m._33), _34(0),
    _41(0),     _42(0),     _43(0),     _44(1)
{
}

math::mat4::mat4(mat3 m, vec3 column, vec3 row, math::scalar corner) :
    _11(m._11), _12(m._12), _13(m._13), _14(column.x),
    _21(m._21), _22(m._22), _23(m._23), _24(column.y),
    _31(m._31), _32(m._32), _33(m._33), _34(column.z),
    _41(row.x), _42(row.y), _43(row.z), _44(corner)
{
}

/* Row and Column accessors */
math::vec2 math::mat2::row1() const { return vec2(_11, _12); }
math::vec2 math::mat2::row2() const { return vec2(_21, _22); }

math::vec2 math::mat2::col1() const { return vec2(_11, _21); }
math::vec2 math::mat2::col2() const { return vec2(_12, _22); }


math::vec3 math::mat3::row1() const { return vec3(_11, _12, _13); }
math::vec3 math::mat3::row2() const { return vec3(_21, _22, _23); }
math::vec3 math::mat3::row3() const { return vec3(_31, _32, _33); }

math::vec3 math::mat3::col1() const { return vec3(_11, _21, _31); }
math::vec3 math::mat3::col2() const { return vec3(_12, _22, _32); }
math::vec3 math::mat3::col3() const { return vec3(_13, _23, _33); }


math::vec4 math::mat4::row1() const { return vec4(_11, _12, _13, _14); }
math::vec4 math::mat4::row2() const { return vec4(_21, _22, _23, _24); }
math::vec4 math::mat4::row3() const { return vec4(_31, _32, _33, _34); }
math::vec4 math::mat4::row4() const { return vec4(_41, _42, _43, _44); }

math::vec4 math::mat4::col1() const { return vec4(_11, _21, _31, _41); }
math::vec4 math::mat4::col2() const { return vec4(_12, _22, _32, _42); }
math::vec4 math::mat4::col3() const { return vec4(_13, _23, _33, _43); }
math::vec4 math::mat4::col4() const { return vec4(_14, _24, _34, _44); }


// Look at
void math::mat4::lookAt(vec3 pos, vec3 targ, vec3 Up) {
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

void math::mat4::orthoProjection(scalar left, scalar right, scalar bottom, scalar top, scalar znear, scalar zfar) {

    *this = mat4(
        vec4(   2.0/(right-left),
                0,
                0,
                0), 

        vec4(   0, 
                2.0/(top-bottom),
                0,
                0), 

        vec4(   0,
                0, 
               -2.0/(zfar-znear),
                0), 

        vec4(  -(right+left) / (right-left),
               -(top+bottom) / (top-bottom),
               -(zfar+znear) / (zfar-znear),
                1.0));
}



/* Define mathematical operations */
math::vec2 math::operator+ (const vec2& A, const vec2& B) {
    return vec2(A.x + B.x, A.y + B.y);
}
math::vec3 math::operator+ (const vec3& A, const vec3& B) {
    return vec3(A.x + B.x, A.y + B.y, A.z + B.z);
}
math::vec4 math::operator+ (const vec4& A, const vec4& B) {
    return vec4(A.x + B.x, A.y + B.y, A.z + B.z, A.w + B.w);
}

math::mat2 math::operator+ (const mat2& A, const mat2& B) {
    return mat2(A.col1() + B.col1(), A.col2() + B.col2());
}
math::mat3 math::operator+ (const mat3& A, const mat3& B) {
    return mat3(A.col1() + B.col1(), A.col2() + B.col2(), A.col3() + B.col3());
}
math::mat4 math::operator+ (const mat4& A, const mat4& B) {
    return mat4(A.col1() + B.col1(), A.col2() + B.col2(), A.col3() + B.col3(), A.col4() + B.col4());
}

math::vec2 math::operator- (const vec2& A, const vec2& B) {
    return vec2(A.x - B.x, A.y - B.y);
}
math::vec3 math::operator- (const vec3& A, const vec3& B) {
    return vec3(A.x - B.x, A.y - B.y, A.z - B.z);
}
math::vec4 math::operator- (const vec4& A, const vec4& B) {
    return vec4(A.x - B.x, A.y - B.y, A.z - B.z, A.w - B.w);
}

math::mat2 math::operator- (const mat2& A, const mat2& B) {
    return mat2(A.col1() - B.col1(), A.col2() - B.col2());
}
math::mat3 math::operator- (const mat3& A, const mat3& B) {
    return mat3(A.col1() - B.col1(), A.col2() - B.col2(), A.col3() - B.col3());
}
math::mat4 math::operator- (const mat4& A, const mat4& B) {
    return mat4(A.col1() - B.col1(), A.col2() - B.col2(), A.col3() - B.col3(), A.col4() - B.col4());
}

math::vec2 math::operator* (const vec2& V, scalar s) {
    return vec2(V.x*s, V.y*s);
}
math::vec3 math::operator* (const vec3& V, scalar s) {
    return vec3(V.x*s, V.y*s, V.z*s);
}
math::vec4 math::operator* (const vec4& V, scalar s) {
    return vec4(V.x*s, V.y*s, V.z*s, V.w*s);
}

math::vec2 math::operator* (scalar s, const vec2& V) {
    return vec2(V.x*s, V.y*s);
}
math::vec3 math::operator* (scalar s, const vec3& V) {
    return vec3(V.x*s, V.y*s, V.z*s);
}
math::vec4 math::operator* (scalar s, const vec4& V) {
    return vec4(V.x*s, V.y*s, V.z*s, V.w*s);
}

math::mat2 math::operator* (const mat2& M, scalar s) {
    return mat2(M.col1()*s, M.col2()*s);
}
math::mat3 math::operator* (const mat3& M, scalar s) {
    return mat3(M.col1()*s, M.col2()*s, M.col3()*s);
}
math::mat4 math::operator* (const mat4& M, scalar s) {
    return mat4(M.col1()*s, M.col2()*s, M.col3()*s, M.col4()*s);
}

math::mat2 math::operator* (scalar s, const mat2& M) {
    return mat2(M.col1()*s, M.col2()*s);
}
math::mat3 math::operator* (scalar s, const mat3& M) {
    return mat3(M.col1()*s, M.col2()*s, M.col3()*s);
}
math::mat4 math::operator* (scalar s, const mat4& M) {
    return mat4(M.col1()*s, M.col2()*s, M.col3()*s, M.col4()*s);
}

math::mat2 math::operator* (const mat2& A, const mat2& B) {
    return mat2(
        vec2(
            A.row1().dot(B.col1()),
            A.row2().dot(B.col1())),
        vec2(
            A.row1().dot(B.col2()),
            A.row2().dot(B.col2()))
    );
}
math::mat3 math::operator* (const mat3& A, const mat3& B) {
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
math::mat4 math::operator* (const mat4& A, const mat4& B) {
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

math::vec2 math::operator* (const mat2& M, const vec2& V) {
    return vec2(
        M.row1().dot(V),
        M.row2().dot(V)
    );
}
math::vec3 math::operator* (const mat3& M, const vec3& V) {
    return vec3(
        M.row1().dot(V),
        M.row2().dot(V),
        M.row3().dot(V)
    );
}
math::vec4 math::operator* (const mat4& M, const vec4& V) {
    return vec4(
        M.row1().dot(V),
        M.row2().dot(V),
        M.row3().dot(V),
        M.row4().dot(V)
    );
}

math::vec2 math::operator/ (const vec2& V, scalar s) {
    return vec2(V.x/s, V.y/s);
}
math::vec3 math::operator/ (const vec3& V, scalar s) {
    return vec3(V.x/s, V.y/s, V.z/s);
}
math::vec4 math::operator/ (const vec4& V, scalar s) {
    return vec4(V.x/s, V.y/s, V.z/s, V.w/s);
}

math::mat2 math::operator/ (const mat2& M, scalar s) {
    return mat2(M.col1()/s, M.col2()/s);
}
math::mat3 math::operator/ (const mat3& M, scalar s) {
    return mat3(M.col1()/s, M.col2()/s, M.col3()/s);
}
math::mat4 math::operator/ (const mat4& M, scalar s) {
    return mat4(M.col1()/s, M.col2()/s, M.col3()/s, M.col4()/s);
}

math::vec2& math::operator+= (vec2& A, const vec2& B) {
    A = vec2(A.x + B.x, A.y + B.y);
    return A;
}
math::vec3& math::operator+= (vec3& A, const vec3& B) {
    A = vec3(A.x + B.x, A.y + B.y, A.z + B.z);
    return A;
}
math::vec4& math::operator+= (vec4& A, const vec4& B) {
    A = vec4(A.x + B.x, A.y + B.y, A.z + B.z, A.w + B.w);
    return A;
}

math::mat2& math::operator+= (mat2& A, const mat2& B) {
    A = mat2(A.col1() + B.col1(), A.col2() + B.col2());
    return A;
}
math::mat3& math::operator+= (mat3& A, const mat3& B) {
    A = mat3(A.col1() + B.col1(), A.col2() + B.col2(), A.col3() + B.col3());
    return A;
}
math::mat4& math::operator+= (mat4& A, const mat4& B) {
    A = mat4(A.col1() + B.col1(), A.col2() + B.col2(), A.col3() + B.col3(), A.col4() + B.col4());
    return A;
}

math::vec2& math::operator-= (vec2& A, const vec2& B) {
    A = vec2(A.x - B.x, A.y - B.y);
    return A;
}
math::vec3& math::operator-= (vec3& A, const vec3& B) {
    A = vec3(A.x - B.x, A.y - B.y, A.z - B.z);
    return A;
}
math::vec4& math::operator-= (vec4& A, const vec4& B) {
    A = vec4(A.x - B.x, A.y - B.y, A.z - B.z, A.w - B.w);
    return A;
}

math::mat2& math::operator-= (mat2& A, const mat2& B) {
    A = mat2(A.col1() - B.col1(), A.col2() - B.col2());
    return A;
}
math::mat3& math::operator-= (mat3& A, const mat3& B) {
    A = mat3(A.col1() - B.col1(), A.col2() - B.col2(), A.col3() - B.col3());
    return A;
}
math::mat4& math::operator-= (mat4& A, const mat4& B) {
    A = mat4(A.col1() - B.col1(), A.col2() - B.col2(), A.col3() - B.col3(), A.col4() - B.col4());
    return A;
}

math::vec2& math::operator*= (vec2& V, scalar s) {
    V = vec2(V.x*s, V.y*s);
    return V;
}
math::vec3& math::operator*= (vec3& V, scalar s) {
    V = vec3(V.x*s, V.y*s, V.z*s);
    return V;
}
math::vec4& math::operator*= (vec4& V, scalar s) {
    V = vec4(V.x*s, V.y*s, V.z*s, V.w*s);
    return V;
}

math::mat2& math::operator*= (mat2& M, scalar s) {
    M = mat2(M.col1()*s, M.col2()*s);
    return M;
}
math::mat3& math::operator*= (mat3& M, scalar s) {
    M = mat3(M.col1()*s, M.col2()*s, M.col3()*s);
    return M;
}
math::mat4& math::operator*= (mat4& M, scalar s) {
    M = mat4(M.col1()*s, M.col2()*s, M.col3()*s, M.col4()*s);
    return M;
}

math::mat2& math::operator*= (mat2& A, const mat2& B) {
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
math::mat3& math::operator*= (mat3& A, const mat3& B) {
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
math::mat4& math::operator*= (mat4& A, const mat4& B) {
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

math::vec2& math::operator/= (vec2& V, scalar s) {
    V = vec2(V.x/s, V.y/s);
    return V;
}
math::vec3& math::operator/= (vec3& V, scalar s) {
    V = vec3(V.x/s, V.y/s, V.z/s);
    return V;
}
math::vec4& math::operator/= (vec4& V, scalar s) {
    V = vec4(V.x/s, V.y/s, V.z/s, V.w/s);
    return V;
}

math::mat2& math::operator/= (mat2& M, scalar s) {
    M = mat2(M.col1()/s, M.col2()/s);
    return M;
}
math::mat3& math::operator/= (mat3& M, scalar s) {
    M = mat3(M.col1()/s, M.col2()/s, M.col3()/s);
    return M;
}
math::mat4& math::operator/= (mat4& M, scalar s) {
    M = mat4(M.col1()/s, M.col2()/s, M.col3()/s, M.col4()/s);
    return M;
}

math::vec2 math::operator- (const vec2& V) {
    return vec2(-V.x, -V.y);
}
math::vec3 math::operator- (const vec3& V) {
    return vec3(-V.x, -V.y, -V.z);
}
math::vec4 math::operator- (const vec4& V) {
    return vec4(-V.x, -V.y, -V.z, -V.w);
}

math::mat2 math::operator- (const mat2& M) {
    return mat2(-M.col1(), -M.col2());
}
math::mat3 math::operator- (const mat3& M) {
    return mat3(-M.col1(), -M.col2(), -M.col3());
}
math::mat4 math::operator- (const mat4& M) {
    return mat4(-M.col1(), -M.col2(), -M.col3(), -M.col4());
}



math::mat3& math::mat3::toYawPitchRoll(scalar yaw, scalar pitch, scalar roll) {
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

math::scalar math::mat3::yaw() {
    return -atan2(_11, _31)*r2d;
}

math::scalar math::mat3::pitch() {
    return asin(_21)*r2d;
}

math::scalar math::mat3::roll() {
    return -atan2(_23, _22)*r2d;
}

math::mat3 math::mat3::getTranspose() {
    return mat3(row1(), row2(), row3());
}


math::mat3 math::createYawPitchRollMatrix(scalar yaw, scalar pitch, scalar roll) {
    // 2-3-1 Body-Fixed Euler-Rotation
    scalar C1 = cos(roll*d2r);  //x Roll
    scalar S1 = sin(roll*d2r);
    scalar C2 = cos(pitch*d2r); //z Pitch
    scalar S2 = sin(pitch*d2r);
    scalar C3 = cos(yaw*d2r);   //y Yaw
    scalar S3 = sin(yaw*d2r);

    mat3 X1 = mat3(
        vec3(1,0,0),
        vec3(0,C1,S1),
        vec3(0,-S1,C1)
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

    mat3 m = Y3*Z2*X1;

    return m;
}

math::mat4 math::createInverseTransform(vec3 Position, vec3 YawPitchRoll, vec3 Scale) {
    mat4 out = mat4(createYawPitchRollMatrix(YawPitchRoll.x, YawPitchRoll.y, YawPitchRoll.z).getTranspose()) *
        mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), 
            vec4(-Position.x/Scale.x, -Position.y/Scale.y, -Position.z/Scale.z, 1));

    return out;
}

void math::vec4normalizeXYZ_remap(math::vec4 &v) {
    scalar f = 1 / vec3(v).length();

    v = f * vec4(-v.z, v.y, v.x, v.w);
}

math::vec3 math::vec4::XYZ() {
    return vec3(x, y, z);
}

math::vec3 math::createYawPitchRoll_Forward(vec3 YawPitchRoll) {
    // 2-3-1 Body-Fixed Euler-Rotation
    scalar C1 = cos(YawPitchRoll.z*d2r);  //x Roll
    scalar S1 = sin(YawPitchRoll.z*d2r);
    scalar C2 = cos(YawPitchRoll.y*d2r); //z Pitch
    scalar S2 = sin(YawPitchRoll.y*d2r);
    scalar C3 = cos(YawPitchRoll.x*d2r);   //y Yaw
    scalar S3 = sin(YawPitchRoll.x*d2r);

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

    return m.col1();
}

math::vec3 math::createYawPitchRoll_Right(vec3 YawPitchRoll) {
    // 2-3-1 Body-Fixed Euler-Rotation
    scalar C1 = cos(YawPitchRoll.z*d2r);  //x Roll
    scalar S1 = sin(YawPitchRoll.z*d2r);
    scalar C2 = cos(YawPitchRoll.y*d2r); //z Pitch
    scalar S2 = sin(YawPitchRoll.y*d2r);
    scalar C3 = cos(YawPitchRoll.x*d2r);   //y Yaw
    scalar S3 = sin(YawPitchRoll.x*d2r);

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

    return m.col3();
}

math::vec3 math::createYawPitchRoll_Up(vec3 YawPitchRoll) {
    // 2-3-1 Body-Fixed Euler-Rotation
    scalar C1 = cos(YawPitchRoll.z*d2r);  //x Roll
    scalar S1 = sin(YawPitchRoll.z*d2r);
    scalar C2 = cos(YawPitchRoll.y*d2r); //z Pitch
    scalar S2 = sin(YawPitchRoll.y*d2r);
    scalar C3 = cos(YawPitchRoll.x*d2r);   //y Yaw
    scalar S3 = sin(YawPitchRoll.x*d2r);

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

    return m.col2();
}




/* ostream function overloads */
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

std::ostream & math::operator<<(std::ostream & os, const vec4 & v)
{
    // TODO: float formatting
    os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ") ";
    return os;
}

std::ostream & math::operator<<(std::ostream & os, const mat4 & m)
{
    // TODO: float formatting
    os << m._11 << " " << m._12 << " " << m._13 << " " << m._14 << " " << std::endl;
    os << m._21 << " " << m._22 << " " << m._23 << " " << m._24 << " " << std::endl;
    os << m._31 << " " << m._32 << " " << m._33 << " " << m._34 << " " << std::endl;
    os << m._41 << " " << m._42 << " " << m._43 << " " << m._44 << " " << std::endl;
    return os;
}