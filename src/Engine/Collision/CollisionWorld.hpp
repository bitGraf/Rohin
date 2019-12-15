#ifndef COLLISION_WORLD_H
#define COLLISION_WORLD_H

#include "CollisionHull.hpp"
#include "CollisionMath.hpp"

#include <vector>

struct RaycastResult {
    f32 t;
    UID_t colliderID;
    vec3 contactPoint;
    vec3 normal;

    vec3 start, end;
};

#define MAX_CONTACTS 8

struct ContactPlane {
    float TOI;
    UID_t colliderID;

    vec3 contact_normal;
    vec3 contact_point;
};

struct ShapecastResult_multi {
    ContactPlane planes[MAX_CONTACTS];
    int numContacts;

    int iters;
    float lowestTOI;
};
struct ShapecastResult {
    int iters;
    float TOI;
    vec3 contact_normal;
    vec3 contact_point;
    UID_t colliderID;
};

#define MAX_POINTS 10
#define MAX_FACES 10
#define MAX_INCREASING_ITS 2

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
    bool m_hit;
};
struct gjk_Output {
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
    bool m_hit;

    Simplex simplices[e_maxSimplices];
    int simplexCount;

    TermCode m_term;
};
struct gjk_Input {
    CollisionHull* hull1;
    CollisionHull* hull2;
};

class CollisionWorld {
public:
    CollisionWorld();

    void Update(double dt);

    RaycastResult Raycast(vec3 start, vec3 end);
    RaycastResult Raycast(vec3 start, vec3 direction, scalar distance);

    RaycastResult Raycast(UID_t target, vec3 start, vec3 end);

    ShapecastResult_multi Shapecast_multi(UID_t id, vec3 vel);
    ShapecastResult Shapecast(UID_t id, vec3 vel);

    scalar TimeOfImpact(CollisionHull* hull1, vec3 vel1, 
        CollisionHull* hull2, vec3 vel2,
        vec3* out_normal, vec3* out_contact_point, 
        int* out_iterations);
    scalar StepGJK(float t, 
        CollisionHull* hull1, vec3 vel1, 
        CollisionHull* hull2, vec3 vel2,
        vec3* point1, vec3* point2,
        float feather_radius);

    UID_t CreateNewCubeHull(ResourceManager* resource, 
        vec3 position, scalar size);
    UID_t CreateNewCubeHull(ResourceManager* resource,
        vec3 position, scalar xSize, scalar ySize, scalar zSize);
    UID_t CreateNewCapsule(ResourceManager* resource, 
        vec3 position, scalar height, scalar radius);
    CollisionHull* getHullFromID(UID_t id);

    /* GJK algorithm */
    void GJK(gjk_Output* output, gjk_Input& input);

//private:
    std::vector<CollisionHull> m_static;
    std::vector<CollisionHull> m_dynamic;
};

extern CollisionWorld cWorld;

#endif
