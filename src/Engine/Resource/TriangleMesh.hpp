#ifndef MODELS_H
#define MODELS_H

#include "glad\glad.h"
#include "GameMath.hpp"
#include "Resource/MemoryManager.hpp"

typedef DataBlock<math::vec4>   vec4Array;
typedef DataBlock<math::vec3>   vec3Array;
typedef DataBlock<math::vec2>   vec2Array;
typedef DataBlock<index_t>      indexArray;

class Triangle {
public:
    index_t indices[3] = { 0,0,0 };
};

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
};

typedef TriangleMesh*   meshRef;

#endif
