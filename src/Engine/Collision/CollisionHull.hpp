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

    vec3 supportPoint(vec3 search_dir);

//private:
    void bufferData();

    vec3Array vertices;
    EdgeArray edges;

    u16 numVerts2Draw;
    GLuint wireframeVAO;
};

#endif
