#include "Material.hpp"

Material_Texture::Material_Texture() {
    index = -1;
    tex_coord = -1;
    value = 0;

    glTexID = 0;
}


Material::Material() {
    // Material Name
    std::string name;

    // Material Textures
    //Material_Texture normalTexture;
    //Material_Texture OcclusionTexture;
    //Material_Texture EmissiveTexture;
    //Material_Texture baseColorTexture;
    //Material_Texture metallicRoughnessTexture;

    // Material Factors
    //math::vec3 emissiveFactor;
    //math::vec4 baseColorFactor;
    f32 metallicFactor = 0;
    f32 roughnessFactor = 0;

    // Other values
    f32 alphaCutoff = 0.5;
    std::string alphaMode;
    bool doubleSided = true;
}