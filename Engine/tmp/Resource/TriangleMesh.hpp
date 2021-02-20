#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include "Engine/Core/GameMath.hpp"
#include "Engine/Resource/MemoryManager.hpp"
#include "glad/glad.h"

struct Triangle {
    index_t indices[3];
    Triangle() : indices{ 0,0,0 } {}
    Triangle(index_t n1, index_t n2, index_t n3) : indices{ n1,n2,n3 } {}
};

struct Edge {
    index_t indices[2];
    Edge() : indices{ 0,0 } {}
    Edge(index_t n1, index_t n2) : indices{ n1,n2 } {}
};

typedef DataBlock<math::vec4>   vec4Array;
typedef DataBlock<math::vec3>   vec3Array;
typedef DataBlock<math::vec2>   vec2Array;
typedef DataBlock<int>          indexArray;
typedef DataBlock<Triangle>     triangleArray;
typedef DataBlock<Edge>         EdgeArray;
typedef DataBlock<Triangle>     FaceArray;

/* 104 bytes */
class TriangleMesh {
public:
    // Size info
    index_t numVerts = 0;
    index_t numFaces = 0;

    // Vertex Data
    vec3Array vertPositions;
    vec3Array vertNormals;
    vec2Array vertTexCoords;
    vec4Array vertTangents;
    vec3Array vertBitangents;

    // Face Data
    indexArray indices;

    // OpenGL info
    GLuint VAO = 0;
    GLuint VBOpos = 0;
    GLuint VBOnorm = 0;
    GLuint VBOtex = 0;
    GLuint VBOtan = 0;
    GLuint VBObitan = 0;
    GLuint EBO = 0;

    // state
    bool initialized = false;
    u8 flag;
};

typedef TriangleMesh*   meshRef;

#endif
