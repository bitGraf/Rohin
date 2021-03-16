#ifndef COLLISION_HULL_H
#define COLLISION_HULL_H

#include "Engine/Core/GameMath.hpp"

using namespace math;
typedef u64 UID_t;

struct Triangle {
    index_t indices[3];
    Triangle() : indices{ 0,0,0 } {}
    Triangle(index_t n1, index_t n2, index_t n3) : indices{ n1,n2,n3 } {}
};

class CollisionHull {
public:
    CollisionHull();

    int GetSupport(vec3 search_dir);
    vec3 GetVertWorldSpace(int index);

    vec3 position;
    mat3 rotation;
    u32 wireframeVAO;

    void bufferData();

    std::vector<math::vec3> vertices;
    std::vector<Triangle> faces;
    float m_radius;

    const UID_t m_hullID;

private:
    /// Get the next globally unique ID
    static UID_t getNextUID();
    static UID_t nextUID;
};

#endif
