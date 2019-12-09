#pragma once

#include "Collision\CollisionMath.hpp"
#include "Collision\CollisionHull.hpp"
#include "Collision\CollisionWorld.hpp"
#include <vector>
#include <cassert>

/*
#define MAX_POINTS 10
#define MAX_FACES 10
#define MAX_INCREASING_ITS 5

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
    void GetWitnessPoints(vec3* point1, vec3* point2, 
        float radius1 = -1, float radius2 = -1);
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
    CollisionHull* polygon1;
    CollisionHull* polygon2;
};*/

void Distance3D(gjk_Output* output, gjk_Input& input);

struct Line {
    vec3 a, b;
};

class ResourceManager;
struct Visualizer {
    void Init(ResourceManager* resource);
    void Step();

    std::vector<Line> m_lines;

    RaycastResult res;

    gjk_Output out;
    gjk_Input in;

};