#include "CollisionHull.hpp"
#include "Resource\ResourceManager.hpp"

CollisionHull::CollisionHull() {

}

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

void CollisionHull::testLoadCube(scalar size, ResourceManager* resource) {
    vertices = resource->reserveDataBlocks<vec3>(8); //8 verts on a cube
    edges = resource->reserveDataBlocks<Edge>(12); //12 edges on a cube

    vertices.data[0] = vec3( size, -size,  size);
    vertices.data[1] = vec3( size, -size, -size);
    vertices.data[2] = vec3(-size, -size, -size);
    vertices.data[3] = vec3(-size, -size,  size);

    vertices.data[4] = vec3( size, size,  size);
    vertices.data[5] = vec3( size, size, -size);
    vertices.data[6] = vec3(-size, size, -size);
    vertices.data[7] = vec3(-size, size,  size);

    edges.data[0] = { 0, 1 };
    edges.data[1] = { 1, 2 };
    edges.data[2] = { 2, 3 };
    edges.data[3] = { 3, 0 };

    edges.data[4] = { 4, 5 };
    edges.data[5] = { 5, 6 };
    edges.data[6] = { 6, 7 };
    edges.data[7] = { 7, 4 };

    edges.data[8] = { 0, 4 };
    edges.data[9] = { 1, 5 };
    edges.data[10] = { 2, 6 };
    edges.data[11] = { 3, 7 };

    bufferData();
}

void CollisionHull::loadVerts(ResourceManager* resource, int count...) {
    vertices = resource->reserveDataBlocks<vec3>(count);

    va_list args;
    va_start(args, count);

    for (int n = 0; n < count; n++) {
        vertices.data[n] = va_arg(args, vec3);
    }

    va_end(args);
}

void CollisionHull::loadEdges(ResourceManager* resource, int count...) {
    edges = resource->reserveDataBlocks<Edge>(count);

    va_list args;
    va_start(args, count);

    for (int n = 0; n < count; n++) {
        edges.data[n] = va_arg(args, Edge);
    }

    va_end(args);
}

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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.blockBytes, edges.data, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
}