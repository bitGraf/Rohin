#include <enpch.hpp>

#include "Engine/Core/GameMath.hpp"

namespace math {
    /* * * * * * * * * * * * * * * * * * * * * * * * * * *
     * Mat2  * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * */

    mat2::mat2() : column1(1, 0), column2(0, 1) {}// identity
    mat2::mat2(scalar f) : column1(f), column2(f) {}// entire matrix one value
    mat2::mat2(const vec2& col1, const vec2& col2) : column1(col1), column2(col2) {}
    mat2::mat2(scalar c11, scalar c22) : column1(c11, 0), column2(0, c22) {}
    mat2::mat2(const mat2& m) : column1(m.column1), column2(m.column2) {}

    void mat2::identity() {
        column1[0] = 1; column2[0] = 0;
        column1[1] = 0; column2[1] = 1;
    }

    void mat2::transpose() {
        scalar tmp = column1[1];
        column1[1] = column2[0];
        column2[0] = tmp;
    }
    mat2 mat2::getTranspose() const {
        mat2 copy = *this;
        copy.transpose();
        return copy;
    }

    const scalar* mat2::ptr() const {
        return column1.ptr();
    }
    vec2& mat2::operator[](std::size_t idx) {
        assert(idx >= 0 && idx < 2);
        return *((&column1) + idx);
    }
    const vec2& mat2::operator[](std::size_t idx) const {
        assert(idx >= 0 && idx < 2);
        return *((&column1) + idx);
    }

    std::ostream & operator<<(std::ostream & os, const mat2 & m)
    {
        // TODO: float formatting
        // [1,2;3,4]
        return os << '[' << m.row1() << ';' << m.row2() << ']';
    }

    vec2 mat2::row1() const { return vec2(column1[0], column2[0]); }
    vec2 mat2::row2() const { return vec2(column1[1], column2[1]); }

    /* operators */
    bool operator== (const mat2& A, const mat2& B) {
        return (A.row1() == B.row1()) &&
            (A.row2() == B.row2());
    }
    mat2 operator+ (const mat2& A, const mat2& B) {
        return mat2(A.column1 + B.column1, A.column2 + B.column2);
    }
    mat2 operator- (const mat2& A, const mat2& B) {
        return mat2(A.column1 - B.column1, A.column2 - B.column2);
    }
    mat2 operator* (const mat2& M, scalar s) {
        return mat2(M.column1*s, M.column2*s);
    }
    mat2 operator* (scalar s, const mat2& M) {
        return mat2(M.column1*s, M.column2*s);
    }
    mat2 operator* (const mat2& A, const mat2& B) {
        return mat2(
            vec2(
                A.row1().dot(B.column1),
                A.row2().dot(B.column1)),
            vec2(
                A.row1().dot(B.column2),
                A.row2().dot(B.column2))
        );
    }
    vec2 operator* (const mat2& M, const vec2& V) {
        return vec2(
            M.row1().dot(V),
            M.row2().dot(V)
        );
    }
    mat2 operator/ (const mat2& M, scalar s) {
        return mat2(M.column1 / s, M.column2 / s);
    }
    mat2& operator+= (mat2& A, const mat2& B) {
        A = mat2(A.column1 + B.column1, A.column2 + B.column2);
        return A;
    }
    mat2& operator-= (mat2& A, const mat2& B) {
        A = mat2(A.column1 - B.column1, A.column2 - B.column2);
        return A;
    }
    mat2& operator*= (mat2& M, scalar s) {
        M = mat2(M.column1*s, M.column2*s);
        return M;
    }
    mat2& operator*= (mat2& A, const mat2& B) {
        A = mat2(
            vec2(
                A.row1().dot(B.column1),
                A.row2().dot(B.column1)),
            vec2(
                A.row1().dot(B.column2),
                A.row2().dot(B.column2))
        );
        return A;
    }
    mat2& operator/= (mat2& M, scalar s) {
        M = mat2(M.column1 / s, M.column2 / s);
        return M;
    }
    mat2 operator- (const mat2& M) {
        return mat2(-M.column1, -M.column2);
    }


    /* * * * * * * * * * * * * * * * * * * * * * * * * * *
     * Mat3  * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * */
    mat3::mat3() : column1(1, 0, 0), column2(0, 1, 0), column3(0, 0, 1) {}// identity
    mat3::mat3(scalar f) : column1(f), column2(f), column3(f) {}// entire matrix one value
    mat3::mat3(scalar c11, scalar c22, scalar c33) :
        column1(c11, 0, 0), column2(0, c22, 0), column3(0, 0, c33) {}
    mat3::mat3(const vec3& col1, const vec3& col2, const vec3& col3) :
        column1(col1), column2(col2), column3(col3) {}
    mat3::mat3(const mat3& m) : column1(m.column1), column2(m.column2), column3(m.column3) {}

    void mat3::identity() {
        column1[0] = 1; column2[0] = 0; column3[0] = 0;
        column1[1] = 0; column2[1] = 1; column3[1] = 0;
        column1[2] = 0; column2[2] = 0; column3[2] = 1;
    }
    void mat3::transpose() {
        scalar tmp;

        tmp = column1[1];
        column1[1] = column2[0];
        column2[0] = tmp;

        tmp = column1[2];
        column1[2] = column3[0];
        column3[0] = tmp;

        tmp = column2[2];
        column2[2] = column3[1];
        column3[1] = tmp;
    }
    mat3 mat3::getTranspose() const {
        mat3 copy = *this;
        copy.transpose();
        return copy;
    }

    const scalar* mat3::ptr() const {
        return column1.ptr();
    }
    vec3& mat3::operator[](std::size_t idx) {
        assert(idx >= 0 && idx < 3);
        return *((&column1) + idx);
    }
    const vec3& mat3::operator[](std::size_t idx) const {
        assert(idx >= 0 && idx < 3);
        return *((&column1) + idx);
    }

    std::ostream & operator<<(std::ostream & os, const mat3 & m)
    {
        // TODO: float formatting
        // [1,2,3;4,5,6;7,8,9]
        return os << '[' << m.row1() << ';' << m.row2() << ';' << m.row3() << ']';
    }

    vec3 mat3::row1() const { return vec3(column1[0], column2[0], column3[0]); }
    vec3 mat3::row2() const { return vec3(column1[1], column2[1], column3[1]); }
    vec3 mat3::row3() const { return vec3(column1[2], column2[2], column3[2]); }

    mat2 mat3::asMat2() const {
        return mat2(vec2(column1[0], column1[1]), vec2(column2[0], column2[1]));
    }

    /* operators */
    bool operator== (const mat3& A, const mat3& B) {
        return (A.row1() == B.row1()) &&
            (A.row2() == B.row2()) &&
            (A.row3() == B.row3());
    }
    mat3 operator+ (const mat3& A, const mat3& B) {
        return mat3(A.column1 + B.column1, A.column2 + B.column2, A.column3 + B.column3);
    }
    mat3 operator- (const mat3& A, const mat3& B) {
        return mat3(A.column1 - B.column1, A.column2 - B.column2, A.column3 - B.column3);
    }
    mat3 operator* (const mat3& M, scalar s) {
        return mat3(M.column1*s, M.column2*s, M.column3*s);
    }
    mat3 operator* (scalar s, const mat3& M) {
        return mat3(M.column1*s, M.column2*s, M.column3*s);
    }
    mat3 operator* (const mat3& A, const mat3& B) {
        return mat3(
            vec3(
                A.row1().dot(B.column1),
                A.row2().dot(B.column1),
                A.row3().dot(B.column1)),
            vec3(
                A.row1().dot(B.column2),
                A.row2().dot(B.column2),
                A.row3().dot(B.column2)),
            vec3(
                A.row1().dot(B.column3),
                A.row2().dot(B.column3),
                A.row3().dot(B.column3))
        );
    }
    vec3 operator* (const mat3& M, const vec3& V) {
        return vec3(
            M.row1().dot(V),
            M.row2().dot(V),
            M.row3().dot(V)
        );
    }
    mat3 operator/ (const mat3& M, scalar s) {
        return mat3(M.column1 / s, M.column2 / s, M.column3 / s);
    }
    mat3& operator+= (mat3& A, const mat3& B) {
        A = mat3(A.column1 + B.column1, A.column2 + B.column2, A.column3 + B.column3);
        return A;
    }
    mat3& operator-= (mat3& A, const mat3& B) {
        A = mat3(A.column1 - B.column1, A.column2 - B.column2, A.column3 - B.column3);
        return A;
    }
    mat3& operator*= (mat3& M, scalar s) {
        M = mat3(M.column1*s, M.column2*s, M.column3*s);
        return M;
    }
    mat3& operator*= (mat3& A, const mat3& B) {
        A = mat3(
            vec3(
                A.row1().dot(B.column1),
                A.row2().dot(B.column1),
                A.row3().dot(B.column1)),
            vec3(
                A.row1().dot(B.column2),
                A.row2().dot(B.column2),
                A.row3().dot(B.column2)),
            vec3(
                A.row1().dot(B.column3),
                A.row2().dot(B.column3),
                A.row3().dot(B.column3))
        );
        return A;
    }
    mat3& operator/= (mat3& M, scalar s) {
        M = mat3(M.column1 / s, M.column2 / s, M.column3 / s);
        return M;
    }
    mat3 operator- (const mat3& M) {
        return mat3(-M.column1, -M.column2, -M.column3);
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * *
     * Mat4  * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * */
    mat4::mat4() :
        column1(1, 0, 0, 0), column2(0, 1, 0, 0), column3(0, 0, 1, 0), column4(0, 0, 0, 1) {}// identity
    mat4::mat4(scalar f) :
        column1(f), column2(f), column3(f), column4(f) {}// entire matrix one value
    mat4::mat4(scalar c11, scalar c22, scalar c33, scalar c44) :
        column1(c11, 0, 0, 0), column2(0, c22, 0, 0), column3(0, 0, c33, 0), column4(0, 0, 0, c44) {}
    mat4::mat4(const vec4& col1, const vec4& col2, const vec4& col3, const vec4& col4) :
        column1(col1), column2(col2), column3(col3), column4(col4) {}
    mat4::mat4(const mat4& m) :
        column1(m.column1), column2(m.column2), column3(m.column3), column4(m.column4) {}
    mat4::mat4(const mat3& m, scalar f) 
        : column1(m.column1,0), column2(m.column2, 0), column3(m.column3, 0), column4(0,0,0,1) {}

    void mat4::identity() {
        column1[0] = 1; column2[0] = 0; column3[0] = 0; column4[0] = 0;
        column1[1] = 0; column2[1] = 1; column3[1] = 0; column4[1] = 0;
        column1[2] = 0; column2[2] = 0; column3[2] = 1; column4[2] = 0;
        column1[3] = 0; column2[3] = 0; column3[3] = 0; column4[3] = 1;
    }
    void mat4::transpose() {
        scalar tmp;

        tmp = column1[1];
        column1[1] = column2[0];
        column2[0] = tmp;

        tmp = column1[2];
        column1[2] = column3[0];
        column3[0] = tmp;

        tmp = column2[2];
        column2[2] = column3[1];
        column3[1] = tmp;

        tmp = column1[3];
        column1[3] = column4[0];
        column4[0] = tmp;

        tmp = column2[3];
        column2[3] = column4[1];
        column4[1] = tmp;

        tmp = column3[3];
        column3[3] = column4[2];
        column4[2] = tmp;
    }

    mat4 mat4::getTranspose() const {
        mat4 copy = *this;
        copy.transpose();
        return copy;
    }

    const scalar* mat4::ptr() const {
        return column1.ptr();
    }
    vec4& mat4::operator[](std::size_t idx) {
        assert(idx >= 0 && idx < 4);
        return *((&column1) + idx);
    }
    const vec4& mat4::operator[](std::size_t idx) const {
        assert(idx >= 0 && idx < 4);
        return *((&column1) + idx);
    }


    std::ostream & operator<<(std::ostream & os, const mat4 & m)
    {
        // TODO: float formatting
        // [1,2,3,4;4,5,6,7;9,10,10,12;13,14,15,16]
        return os << '[' << m.row1() << ';' << m.row2()
            << ';' << m.row3() << ';' << m.row4() << ']';
    }

    vec4 mat4::row1() const { return vec4(column1[0], column2[0], column3[0], column4[0]); }
    vec4 mat4::row2() const { return vec4(column1[1], column2[1], column3[1], column4[1]); }
    vec4 mat4::row3() const { return vec4(column1[2], column2[2], column3[2], column4[2]); }
    vec4 mat4::row4() const { return vec4(column1[3], column2[3], column3[3], column4[3]); }

    mat2 mat4::asMat2() const {
        return mat2(vec2(column1[0], column1[1]), vec2(column2[0], column2[1]));
    }
    mat3 mat4::asMat3() const {
        return mat3(vec3(column1[0], column1[1], column1[2]), vec3(column2[0], column2[1], column2[2]), vec3(column3[0], column3[1], column3[2]));
    }

    /* operators */
    bool operator== (const mat4& A, const mat4& B) {
        return (A.row1() == B.row1()) &&
            (A.row2() == B.row2()) &&
            (A.row3() == B.row3()) &&
            (A.row4() == B.row4());
    }
    mat4 operator+ (const mat4& A, const mat4& B) {
        return mat4(A.column1 + B.column1, A.column2 + B.column2, A.column3 + B.column3, A.column4 + B.column4);
    }
    mat4 operator- (const mat4& A, const mat4& B) {
        return mat4(A.column1 - B.column1, A.column2 - B.column2, A.column3 - B.column3, A.column4 - B.column4);
    }
    mat4 operator* (const mat4& M, scalar s) {
        return mat4(M.column1*s, M.column2*s, M.column3*s, M.column4*s);
    }
    mat4 operator* (scalar s, const mat4& M) {
        return mat4(M.column1*s, M.column2*s, M.column3*s, M.column4*s);
    }
    mat4 operator* (const mat4& A, const mat4& B) {
        return mat4(
            vec4(
                A.row1().dot(B.column1),
                A.row2().dot(B.column1),
                A.row3().dot(B.column1),
                A.row4().dot(B.column1)),
            vec4(
                A.row1().dot(B.column2),
                A.row2().dot(B.column2),
                A.row3().dot(B.column2),
                A.row4().dot(B.column2)),
            vec4(
                A.row1().dot(B.column3),
                A.row2().dot(B.column3),
                A.row3().dot(B.column3),
                A.row4().dot(B.column3)),
            vec4(
                A.row1().dot(B.column4),
                A.row2().dot(B.column4),
                A.row3().dot(B.column4),
                A.row4().dot(B.column4))
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
    mat4 operator/ (const mat4& M, scalar s) {
        return mat4(M.column1 / s, M.column2 / s, M.column3 / s, M.column4 / s);
    }
    mat4& operator+= (mat4& A, const mat4& B) {
        A = mat4(A.column1 + B.column1, A.column2 + B.column2, A.column3 + B.column3, A.column4 + B.column4);
        return A;
    }
    mat4& operator-= (mat4& A, const mat4& B) {
        A = mat4(A.column1 - B.column1, A.column2 - B.column2, A.column3 - B.column3, A.column4 - B.column4);
        return A;
    }
    mat4& operator*= (mat4& M, scalar s) {
        M = mat4(M.column1*s, M.column2*s, M.column3*s, M.column4*s);
        return M;
    }
    mat4& operator*= (mat4& A, const mat4& B) {
        A = mat4(
            vec4(
                A.row1().dot(B.column1),
                A.row2().dot(B.column1),
                A.row3().dot(B.column1),
                A.row4().dot(B.column1)),
            vec4(
                A.row1().dot(B.column2),
                A.row2().dot(B.column2),
                A.row3().dot(B.column2),
                A.row4().dot(B.column2)),
            vec4(
                A.row1().dot(B.column3),
                A.row2().dot(B.column3),
                A.row3().dot(B.column3),
                A.row4().dot(B.column3)),
            vec4(
                A.row1().dot(B.column4),
                A.row2().dot(B.column4),
                A.row3().dot(B.column4),
                A.row4().dot(B.column4))
        );
        return A;
    }
    mat4& operator/= (mat4& M, scalar s) {
        M = mat4(M.column1 / s, M.column2 / s, M.column3 / s, M.column4 / s);
        return M;
    }
    mat4 operator- (const mat4& M) {
        return mat4(-M.column1, -M.column2, -M.column3, -M.column4);
    }
}