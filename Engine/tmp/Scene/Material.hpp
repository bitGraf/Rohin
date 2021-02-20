#ifndef MATERIAL_H
#define MATERIAL_H

#include "glad/glad.h"
#include "Engine/Core/GameMath.hpp"
#include "Engine/Resource/MemoryManager.hpp"

struct Material_Texture {
    Material_Texture();

    int index;
    //int tex_coord;

    GLuint glTexID;
    int width, height, nrChannels, channelBits;

    GLint internalFormat;
    GLenum format;

    DataBlock<unsigned char> data;
};

class Material {
public:
    Material();

    // Material name
    std::string name;

    // Material Textures
    Material_Texture baseColorTexture;
    Material_Texture normalTexture;
    Material_Texture amrTexture;
    Material_Texture emissiveTexture;

    // Material Factors
    math::vec3 emissiveFactor;
    math::vec4 baseColorFactor;
    f32 metallicFactor;
    f32 roughnessFactor;

    // Flags
    char flag;
    bool initialised;
};

typedef Material*       materialRef;

#endif

/*
#ifndef MATERIAL_H
#define MATERIAL_H

#include "glad/glad.h"
#include "GameMath.hpp"
#include "Resource/MemoryManager.hpp"

struct Material_Texture {
    Material_Texture();

    int index;
    int tex_coord;
    //f32 value;

    GLuint glTexID;
    int width, height, nrChannels, channelBits;

    GLint internalFormat;
    GLenum format;

    DataBlock<unsigned char> data;
};

class Material {
public:
    Material();

    // Material name
    std::string name;

    // Material Textures
    Material_Texture baseColorTexture;
    Material_Texture normalTexture;
    Material_Texture amrTexture;
    Material_Texture emissiveTexture;

    // Material Factors
    math::vec3 emissiveFactor;
    math::vec4 baseColorFactor;
    f32 metallicFactor;
    f32 roughnessFactor;

    // Other values
    f32 alphaCutoff;
    std::string alphaMode;
    bool doubleSided;
};

typedef Material*       materialRef;

#endif
*/