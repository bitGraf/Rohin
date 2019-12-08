#pragma once

#include "Collision\CollisionMath.hpp"
#include <vector>
#include <cassert>

#define MAX_POINTS 10
#define MAX_FACES 10

struct Tri {
    int a, b, c;
    Tri() { a = b = c = 0; }
    Tri(int _a, int _b, int _c) : a(_a), b(_b), c(_c) {}
};

struct Poly {
    int GetSupport(const vec3& d);

    vec3 m_points[MAX_POINTS];
    int m_count;

    Tri faces[MAX_FACES];
    int numFaces;
};

struct simplexVertex {
    vec3 point1;
    vec3 point2;
    vec3 point;
    float u;
    int index1;
    int index2;
};

struct Simplex {
    vec3 GetSearchDirection();
    void GetWitnessPoints(vec3* point1, vec3* point2);
    float GetDistance();
    void Solve2(const vec3& P);
    void Solve3(const vec3& P);
    void Solve4(const vec3& P);

    simplexVertex m_vertexA, m_vertexB, m_vertexC, m_vertexD;
    float m_distance;
    int m_count;
};

struct Output {
    enum {
        e_maxSimplices = 20
    };

    enum TermCode {
        e_intersect = 0,
        e_distanceIncrease,
        e_zeroSearch,
        e_duplicate,
        e_maxIterations
    };

    vec3 point1;
    vec3 point2;
    float distance;
    int iterations;

    Simplex simplices[e_maxSimplices];
    int simplexCount;

    TermCode m_term;
};

struct gjk_Input {
    Poly polygon1;
    Poly polygon2;
};

void Distance3D(Output* output, gjk_Input& input);

struct Visualizer {
    void Init();
    void Step();

    vec3 line[2];

    Output out;
    gjk_Input in;
    int currStep;

};