#ifndef MATERIAL_H
#define MATERIAL_H

#include "glad\glad.h"
#include "GameMath.hpp"

struct Material_Texture {
    Material_Texture();

    int index;
    int tex_coord;
    f32 value;
};

class Material {
public:
    Material();

    // Material name
    std::string name;

    // Material Textures
    Material_Texture normalTexture;
    Material_Texture OcclusionTexture;
    Material_Texture EmissiveTexture;
    Material_Texture baseColorTexture;
    Material_Texture metallicRoughnessTexture;

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
