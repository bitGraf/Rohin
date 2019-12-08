#ifndef COLLISION_HULL_H
#define COLLISION_HULL_H

#include "GameMath.hpp"
#include "Resource\TriangleMesh.hpp"

using namespace math;
class ResourceManager;

class CollisionHull {
public:
    CollisionHull();

    void testLoadCube(scalar size, ResourceManager* resource);

    void loadVerts(ResourceManager* resource, int count...);
    void loadEdges(ResourceManager* resource, int count...);

    int GetSupport(vec3 search_dir);
    vec3 GetVertWorldSpace(int index);

    vec3 position;
    mat3 rotation;

    u16 numVerts2Draw;
    GLuint wireframeVAO;

//private:
    void bufferData();

    vec3Array vertices;
    EdgeArray edges;
};

#endif
