#include "CollisionHull.hpp"
#include "Resource\ResourceManager.hpp"

CollisionHull::CollisionHull() {

}

vec3 CollisionHull::supportPoint(vec3 search_dir) {
    /* This ALL happens inside model space */
    scalar maxDist = -1;
    int maxIndex = -1;
    for (int n = 0; n < vertices.m_numElements; n++) {
        auto v = &vertices.data[n];
        scalar dist = v->dot(search_dir);

        if (dist > maxDist) {
            maxDist = dist;
            maxIndex = n;
        }
    }

    assert(maxIndex >= 0);
    return vertices.data[maxIndex];
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