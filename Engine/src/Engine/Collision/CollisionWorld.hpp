#ifndef COLLISION_WORLD_H
#define COLLISION_WORLD_H

#include "Engine/Core/Base.hpp"
#include "Engine/Collision/CollisionHull.hpp"

namespace rh {

    struct RaycastResult {
        f32 t;
        UID_t colliderID;
        laml::Vec3 contactPoint;
        laml::Vec3 normal;

        laml::Vec3 start, end;
    };

#define MAX_CONTACTS 8

    struct ContactPlane {
        float TOI;
        UID_t colliderID;

        laml::Vec3 contact_normal;
        laml::Vec3 contact_point;
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
        laml::Vec3 contact_normal;
        laml::Vec3 contact_point;
        UID_t colliderID;
    };

#define MAX_POINTS 10
#define MAX_FACES 10
#define MAX_INCREASING_ITS 2

    struct simplexVertex {
        laml::Vec3 point1;
        laml::Vec3 point2;
        laml::Vec3 point;
        float u;
        int index1;
        int index2;
    };
    struct Simplex {
        laml::Vec3 GetSearchDirection();
        void GetWitnessPoints(laml::Vec3* point1, laml::Vec3* point2,
            float radius1 = -1, float radius2 = -1);
        float GetDistance();
        void Solve2(const laml::Vec3& P);
        void Solve3(const laml::Vec3& P);
        void Solve4(const laml::Vec3& P);

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

        laml::Vec3 point1;
        laml::Vec3 point2;
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

        RaycastResult Raycast(laml::Vec3 start, laml::Vec3 end);
        RaycastResult Raycast(laml::Vec3 start, laml::Vec3 direction, laml::Scalar distance);

        RaycastResult Raycast(UID_t target, laml::Vec3 start, laml::Vec3 end);

        ShapecastResult_multi Shapecast_multi(UID_t id, laml::Vec3 vel);
        ShapecastResult Shapecast(UID_t id, laml::Vec3 vel);

        laml::Scalar TimeOfImpact(CollisionHull* hull1, laml::Vec3 vel1,
            CollisionHull* hull2, laml::Vec3 vel2,
            laml::Vec3* out_normal, laml::Vec3* out_contact_point,
            int* out_iterations);
        laml::Scalar StepGJK(float t,
            CollisionHull* hull1, laml::Vec3 vel1,
            CollisionHull* hull2, laml::Vec3 vel2,
            laml::Vec3* point1, laml::Vec3* point2,
            float feather_radius);

        UID_t CreateNewCubeHull(
            laml::Vec3 position, laml::Scalar size);
        UID_t CreateNewCubeHull(
            laml::Vec3 position, laml::Scalar xSize, laml::Scalar ySize, laml::Scalar zSize);
        UID_t CreateNewCapsule(
            laml::Vec3 position, laml::Scalar height, laml::Scalar radius);
        CollisionHull* getHullFromID(UID_t id);

        /* GJK algorithm */
        void GJK(gjk_Output* output, gjk_Input& input);

        //private:
        std::vector<CollisionHull> m_static;
        std::vector<CollisionHull> m_dynamic;
    };
}

#endif
