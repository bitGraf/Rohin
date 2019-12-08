#ifndef COLLISION_HULL_H
#define COLLISION_HULL_H

#include "GameMath.hpp"
#include "Resource\TriangleMesh.hpp"

using namespace math;
class ResourceManager;

class CollisionHull {
public:
    CollisionHull();

    void loadVerts(ResourceManager* resource, int count...);
    void loadFaces(ResourceManager* resource, int count...);

    int GetSupport(vec3 search_dir);
    vec3 GetVertWorldSpace(int index);

    vec3 position;
    mat3 rotation;
    GLuint wireframeVAO;

//private:
    void bufferData();

    vec3Array vertices;
    FaceArray faces;
};

#endif
