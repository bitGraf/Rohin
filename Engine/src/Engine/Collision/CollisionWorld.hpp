#ifndef COLLISION_WORLD_H
#define COLLISION_WORLD_H

#include "Engine/Core/Base.hpp"
#include "Engine/Collision/CollisionHull.hpp"

namespace rh {

    struct RaycastResult {
        f32 t;
        UID_t colliderID;
        math::vec3 contactPoint;
        math::vec3 normal;

        math::vec3 start, end;
    };

#define MAX_CONTACTS 8

    struct ContactPlane {
        float TOI;
        UID_t colliderID;

        math::vec3 contact_normal;
        math::vec3 contact_point;
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
        math::vec3 contact_normal;
        math::vec3 contact_point;
        UID_t colliderID;
    };

#define MAX_POINTS 10
#define MAX_FACES 10
#define MAX_INCREASING_ITS 2

    struct simplexVertex {
        math::vec3 point1;
        math::vec3 point2;
        math::vec3 point;
        float u;
        int index1;
        int index2;
    };
    struct Simplex {
        math::vec3 GetSearchDirection();
        void GetWitnessPoints(math::vec3* point1, math::vec3* point2,
            float radius1 = -1, float radius2 = -1);
        float GetDistance();
        void Solve2(const math::vec3& P);
        void Solve3(const math::vec3& P);
        void Solve4(const math::vec3& P);

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

        math::vec3 point1;
        math::vec3 point2;
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

        RaycastResult Raycast(math::vec3 start, math::vec3 end);
        RaycastResult Raycast(math::vec3 start, math::vec3 direction, math::scalar distance);

        RaycastResult Raycast(UID_t target, math::vec3 start, math::vec3 end);

        ShapecastResult_multi Shapecast_multi(UID_t id, math::vec3 vel);
        ShapecastResult Shapecast(UID_t id, math::vec3 vel);

        math::scalar TimeOfImpact(CollisionHull* hull1, math::vec3 vel1,
            CollisionHull* hull2, math::vec3 vel2,
            math::vec3* out_normal, math::vec3* out_contact_point,
            int* out_iterations);
        math::scalar StepGJK(float t,
            CollisionHull* hull1, math::vec3 vel1,
            CollisionHull* hull2, math::vec3 vel2,
            math::vec3* point1, math::vec3* point2,
            float feather_radius);

        UID_t CreateNewCubeHull(
            math::vec3 position, math::scalar size);
        UID_t CreateNewCubeHull(
            math::vec3 position, math::scalar xSize, math::scalar ySize, math::scalar zSize);
        UID_t CreateNewCapsule(
            math::vec3 position, math::scalar height, math::scalar radius);
        CollisionHull* getHullFromID(UID_t id);

        /* GJK algorithm */
        void GJK(gjk_Output* output, gjk_Input& input);

        //private:
        std::vector<CollisionHull> m_static;
        std::vector<CollisionHull> m_dynamic;
    };
}

#endif
