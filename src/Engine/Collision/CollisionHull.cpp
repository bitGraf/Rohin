#include "CollisionHull.hpp"
#include "Resource\ResourceManager.hpp"

CollisionHull::CollisionHull() :
    m_hullID(getNextUID()),
    m_radius(0.0f)
{}

int CollisionHull::GetSupport(vec3 search_dir) {
    /* This ALL happens inside model space */
    scalar maxDist = vertices.data[0].dot(search_dir);
    int maxIndex = 0;
    for (int n = 0; n < vertices.m_numElements; n++) {
        auto v = &vertices.data[n];
        scalar dist = v->dot(search_dir);

        if (dist > maxDist) {
            maxDist = dist;
            maxIndex = n;
        }
    }

    assert(maxIndex >= 0);
    return maxIndex;
}

vec3 CollisionHull::GetVertWorldSpace(int index) {
    vec3 local = vertices.data[index];
    return rotation * local + position;
}

/*
void CollisionHull::loadVerts(ResourceManager* resource, int count...) {
    vertices = resource->reserveDataBlocks<vec3>(count);

    va_list args;
    va_start(args, count);

    for (int n = 0; n < count; n++) {
        vertices.data[n] = va_arg(args, vec3);
    }

    va_end(args);
}

void CollisionHull::loadFaces(ResourceManager* resource, int count...) {
    faces = resource->reserveDataBlocks<Triangle>(count);

    va_list args;
    va_start(args, count);

    for (int n = 0; n < count; n++) {
        faces.data[n] = va_arg(args, Triangle);
    }

    va_end(args);
}
*/

void CollisionHull::bufferData() {
    glGenVertexArrays(1, &wireframeVAO);
    glBindVertexArray(wireframeVAO);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.blockBytes, vertices.data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertices.m_elementSize, (void*)0);
    glEnableVertexAttribArray(0);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.blockBytes, faces.data, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
}





UID_t CollisionHull::nextUID = 0;
UID_t CollisionHull::getNextUID() {
    return ++nextUID; // starts at 1
}