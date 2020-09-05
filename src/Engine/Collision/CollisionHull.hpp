#ifndef COLLISION_HULL_H
#define COLLISION_HULL_H

#include "GameMath.hpp"
#include "Resource/TriangleMesh.hpp"

using namespace math;
typedef u64 UID_t;

class CollisionHull {
public:
    CollisionHull();

    //void loadVerts(ResourceManager* resource, int count...);
    //void loadFaces(ResourceManager* resource, int count...);

    int GetSupport(vec3 search_dir);
    vec3 GetVertWorldSpace(int index);

    vec3 position;
    mat3 rotation;
    GLuint wireframeVAO;

//private:
    void bufferData();

    vec3Array vertices;
    FaceArray faces;
    float m_radius;

    const UID_t m_hullID;

private:
    /// Get the next globally unique ID
    static UID_t getNextUID();
    static UID_t nextUID;
};

#endif
